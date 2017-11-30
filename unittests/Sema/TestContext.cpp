//===--- TestContext.cpp - Helper for setting up ASTContexts --------------===//
//
// This source file is part of the Swift.org open source project
//
// Copyright (c) 2014 - 2017 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See https://swift.org/LICENSE.txt for license information
// See https://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//
//===----------------------------------------------------------------------===//

#include "TestContext.h"
#include "swift/AST/Module.h"
#include "swift/AST/ModuleLoader.h"
#include "swift/Strings.h"
#include "swift/ClangImporter/ClangImporter.h"
#include "swift/ClangImporter/ClangImporterOptions.h"

using namespace swift;
using namespace swift::unittest;


static void declareOptionalType(ASTContext &ctx, SourceFile *fileForLookups,
                                Identifier name) {
  auto wrapped = new (ctx) GenericTypeParamDecl(fileForLookups,
                                                ctx.getIdentifier("Wrapped"),
                                                SourceLoc(), /*depth*/0,
                                                /*index*/0);
  auto params = GenericParamList::create(ctx, SourceLoc(), wrapped,
                                         SourceLoc());
  auto decl = new (ctx) EnumDecl(SourceLoc(), name, SourceLoc(),
                                 /*inherited*/{}, params, fileForLookups);
  wrapped->setDeclContext(decl);
  fileForLookups->Decls.push_back(decl);
}

TestContext::TestContext(ShouldDeclareOptionalTypes optionals)
    : Ctx(LangOpts, SearchPathOpts, SourceMgr, Diags) {
  auto stdlibID = Ctx.getIdentifier(STDLIB_NAME);
  auto *module = ModuleDecl::create(stdlibID, Ctx);
  Ctx.LoadedModules[stdlibID] = module;
  ClangImporterOptions ClangImporterOpts;
  DependencyTracker *DepTracker = nullptr;
        
  auto clangImporter = ClangImporter::create(Ctx, ClangImporterOpts,
                              "test",
                              DepTracker);
  Ctx.addModuleLoader(std::move(clangImporter), /*isClang*/true);

  using ImplicitModuleImportKind = SourceFile::ImplicitModuleImportKind;
  FileForLookups = new (Ctx) SourceFile(*module, SourceFileKind::Library,
                                        /*buffer*/None,
                                        ImplicitModuleImportKind::Stdlib,
                                        /*keeps token*/false);
  module->addFile(*FileForLookups);

  FileForLookups = new (Ctx) SourceFile(*module, SourceFileKind::Library,
                                        /*buffer*/None,
                                        ImplicitModuleImportKind::Builtin,
                                        /*keeps token*/false);
  module->addFile(*FileForLookups);

  if (optionals == DeclareOptionalTypes) {
    declareOptionalType(Ctx, FileForLookups, Ctx.getIdentifier("Optional"));
    declareOptionalType(Ctx, FileForLookups,
                        Ctx.getIdentifier("ImplicitlyUnwrappedOptional"));
  }
}
