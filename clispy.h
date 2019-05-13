enum Type {
    Number,
    Symbol,
    List
};

struct ParseTree {
    enum Type type;
    struct ParseTree *children;
};
