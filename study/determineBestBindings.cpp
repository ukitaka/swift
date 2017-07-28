// T1
auto tv = CS.createTypeVariable(CS.getConstraintLocator(expr),
                                TVO_PrefersSubtypeBinding |
                                TVO_CanBindToInOut);
// T2
TypeVariableType *TypeVariableType::getNew(const ASTContext &C, unsigned ID, Args &&...args) {
  // Allocate memory
  void *mem = C.Allocate(sizeof(TypeVariableType) + sizeof(Implementation),
  alignof(TypeVariableType),
  AllocationArena::ConstraintSolver);
  // Construct the type variable.
  auto *result = ::new (mem) TypeVariableType(C, ID);
  // Construct the implementation object.
  new (result+1) TypeVariableType::Implementation(std::forward<Args>(args)...);
  return result;
}

// T2
