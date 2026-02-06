#[derive(Debug, Clone, PartialEq)]
pub enum Operation {
    Add,
    Substract,
    Multiply,
    Divide,
    Modulus,
}

#[derive(Debug, Clone, PartialEq)]
pub enum Expression {
    Number(f64),
    BinaryOp {
        left: Box<Expression>,
        op: Operation,
        right: Box<Expression>,
    },
}
