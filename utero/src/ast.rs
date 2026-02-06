#[derive(Debug, Clone, PartialEq)]
pub enum Operation {
    Add,
    Substract,
    Multiply,
    Divide,
    Modulus,
    Equals,
    NotEquals,
    LessThan,
    GreaterThan,
    LessThanEquals,
    GreaterThanEquals,
}

#[derive(Debug, Clone, PartialEq)]
pub enum Expression {
    Number(f64),
    String(String),
    Bool(bool),
    Variable(String),
    BinaryOp {
        left: Box<Expression>,
        op: Operation,
        right: Box<Expression>,
    },
    FnCall {
        callee: String,
        args: Vec<Expression>,
    },
}

#[derive(Debug, Clone, PartialEq)]
pub enum Statement {
    Assign {
        name: String,
        value: Expression,
    },
    Print(Expression),
    While {
        condition: Expression,
        body: Box<Statement>,
    },
    If {
        condition: Expression,
        then_: Box<Statement>,
        else_: Option<Box<Statement>>,
    },
}
