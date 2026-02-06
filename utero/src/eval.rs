use crate::ast::{Expression, Operation};

pub fn evaluate(exp: Expression) -> f64 {
    match exp {
        Expression::Number(n) => n,
        Expression::BinaryOp { left, op, right } => {
            let l = evaluate(*left);
            let r = evaluate(*right);
            match op {
                Operation::Add => l + r,
                Operation::Substract => l - r,
                Operation::Multiply => l * r,
                Operation::Divide => l / r,
                Operation::Modulus => l % r,
            }
        }
    }
}
