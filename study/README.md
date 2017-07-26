## Build

```
utils/build-script -R >/dev/null 2>&1
```

## ASTWalkerの挙動

https://github.com/apple/swift/blob/master/lib/AST/ASTWalker.cpp#L1029-L1059

```cpp
Expr *doIt(Expr *E) {
    auto Pre = Walker.walkToExprPre(E);
    if (!Pre.first || !Pre.second)
        return Pre.second;

    E = visit(Pre.second);

    if (E) E = Walker.walkToExprPost(E);

    return E;
}
```


