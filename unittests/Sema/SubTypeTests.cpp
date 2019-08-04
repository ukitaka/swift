#include "swift/AST/ASTContext.h"
#include "swift/AST/Decl.h"
#include "swift/AST/DeclContext.h"
#include "swift/AST/Types.h"
#include "swift/AST/ProtocolConformance.h"
#include "TypeChecker.h"
#include "TestContext.h"
#include "gtest/gtest.h"
#include <iostream>

using namespace swift;
using namespace swift::unittest;

static TestContext C{DeclareOptionalTypes};
static ModuleDecl* MD = ModuleDecl::create(C.Ctx.getIdentifier("Test"), C.Ctx);
static DeclContext DC(DeclContextKind::TopLevelCodeDecl, MD);
static TypeChecker *TC = new TypeChecker(C.Ctx, C.Diags);

TEST(Sema, SubClass) {
    auto *baseClass = C.makeNominal<ClassDecl>("Base");
    Type baseTy = baseClass->getDeclaredInterfaceType();
    auto *subClass = C.makeNominal<ClassDecl>("Sub");
    subClass->setSuperclass(baseTy);
    Type subTy = subClass->getDeclaredInterfaceType();
    EXPECT_TRUE(TC->isSubtypeOf(subTy, baseTy, &DC));
    EXPECT_TRUE(TC->isSubclassOf(subTy, baseTy, &DC));
    EXPECT_TRUE(TC->isConvertibleTo(subTy, baseTy, &DC));
}

TEST(Sema, Optional) {
    auto *aStruct = C.makeNominal<StructDecl>("A");
    Type aTy = aStruct->swift::TypeDecl::getDeclaredInterfaceType();
    Type optATy = OptionalType::get(aTy);
    EXPECT_TRUE(TC->isSubtypeOf(aTy, optATy, &DC));
    EXPECT_TRUE(TC->isConvertibleTo(aTy, optATy, &DC));
    // ERROR
    // EXPECT_FALSE(TC->isSubclassOf(aTy, optATy, &DC));
}

TEST(Sema, Protocol) {
    auto *animalProtocol = C.makeProtocol(&DC, "Animal");
    ProtocolType *animalTy = ProtocolType::get(animalProtocol, Type(), C.Ctx);
    EXPECT_TRUE(animalTy->getKind() == TypeKind::Protocol);
    Type ty = animalTy;

    auto *dogStruct = C.makeNominal<StructDecl>("Dog");
    Type dogTy = dogStruct->getDeclaredInterfaceType();

    auto conformance = C.Ctx.getConformance(dogTy, animalProtocol, SourceLoc(), dogStruct, ProtocolConformanceState::Complete);
    dogStruct->registerProtocolConformance(conformance);

    EXPECT_TRUE(TC->conformsToProtocol(dogTy, animalProtocol, &DC, ConformanceCheckFlags::InExpression));
    EXPECT_FALSE(TC->conformsToProtocol(animalTy, animalProtocol, &DC, ConformanceCheckFlags::InExpression));
    EXPECT_TRUE(TC->isSubtypeOf(dogTy, ty, &DC));
    EXPECT_TRUE(TC->isConvertibleTo(dogTy, ty, &DC));
}



TEST(Sema, Type) {
    auto *animalProtocol = C.makeProtocol(&DC, "Animal");
    ProtocolType *animalTy = ProtocolType::get(animalProtocol, Type(), C.Ctx);
    Type ty = animalTy;
    EXPECT_TRUE(animalTy->getKind() == TypeKind::Protocol);
    EXPECT_TRUE(ty->getKind() == TypeKind::Protocol);
    EXPECT_TRUE(animalTy->isExistentialType());
    EXPECT_TRUE(ty->isExistentialType());
}

TEST(Sema, ThrowFunc) {
    auto InputType = C.makeNominal<StructDecl>("A")->getDeclaredType();
    auto ResultType = C.makeNominal<StructDecl>("B")->getDeclaredType();
    const AnyFunctionType::ExtInfo InfoForThrowFunc = AnyFunctionType::ExtInfo(AnyFunctionType::Representation::Swift, true);
    const AnyFunctionType::ExtInfo InfoForNonThrowFunc = AnyFunctionType::ExtInfo(AnyFunctionType::Representation::Swift, false);

    auto NonThrowFuncType = FunctionType::get(InputType, ResultType, InfoForNonThrowFunc);
    auto ThrowFuncType = FunctionType::get(InputType, ResultType, InfoForThrowFunc);
    EXPECT_FALSE(TC->isSubtypeOf(ThrowFuncType, NonThrowFuncType, &DC));
    EXPECT_TRUE(TC->isSubtypeOf(NonThrowFuncType, ThrowFuncType, &DC));
}
