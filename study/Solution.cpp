class Solution {
  ConstraintSystem *constraintSystem;
  Score FixedScore;

public:
  Solution(ConstraintSystem &cs, const Score &score)
    : constraintSystem(&cs), FixedScore(score) {}

  size_t getTotalMemory() const;

  ConstraintSystem &getConstraintSystem() const { return *constraintSystem; }

  llvm::SmallDenseMap<TypeVariableType *, Type> typeBindings;

  llvm::SmallDenseMap<ConstraintLocator *, SelectedOverload> overloadChoices;

  llvm::SmallDenseMap<std::pair<CanType, CanType>, ConversionRestrictionKind>
    ConstraintRestrictions;

  llvm::SmallVector<std::pair<Fix, ConstraintLocator *>, 4> Fixes;

  llvm::SmallDenseMap<ConstraintLocator *, unsigned> DisjunctionChoices;

  llvm::SmallDenseMap<ConstraintLocator *, ArrayRef<OpenedType>> OpenedTypes;

  llvm::SmallDenseMap<ConstraintLocator *, ArchetypeType *>
    OpenedExistentialTypes;

  llvm::SmallPtrSet<ConstraintLocator *, 8> DefaultedConstraints;

  llvm::SmallVector<std::pair<ConstraintLocator *, ProtocolConformanceRef>, 8>
      Conformances;

  Type simplifyType(Type type) const;

  Expr *coerceToType(Expr *expr, Type toType,
                     ConstraintLocator *locator,
                     bool ignoreTopLevelInjection = false,
                     Optional<Pattern*> typeFromPattern = None) const;

  Expr *convertBooleanTypeToBuiltinI1(Expr *expr,
                                      ConstraintLocator *locator) const;

  Expr *convertOptionalToBool(Expr *expr, ConstraintLocator *locator) const;

  void computeSubstitutions(GenericSignature *sig,
                            ConstraintLocator *locator,
                            SmallVectorImpl<Substitution> &substitutions) const;

  void computeSubstitutions(GenericSignature *sig,
                            ConstraintLocatorBuilder locator,
                            SmallVectorImpl<Substitution> &substitutions) const;

  unsigned getDisjunctionChoice(ConstraintLocator *locator) const {
    assert(DisjunctionChoices.count(locator));
    return DisjunctionChoices.find(locator)->second;
  }

  const Score &getFixedScore() const { return FixedScore; }

  Score &getFixedScore() { return FixedScore; }

  Type getFixedType(TypeVariableType *typeVar) const;

  ConcreteDeclRef resolveLocatorToDecl(ConstraintLocator *locator) const;
};
