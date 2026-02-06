use crate::ast::{Expression, Operation};
use crate::lexer::Token;
use std::iter::Peekable;
use std::slice::Iter;

pub struct Parser<'a> {
    tokens: Peekable<Iter<'a, Token>>,
}

impl<'a> Parser<'a> {
    pub fn new(tokens: &'a [Token]) -> Self {
        Self {
            tokens: tokens.iter().peekable(),
        }
    }

    pub fn parse(&mut self) -> Expression {
        self.parse_term()
    }

    fn parse_term(&mut self) -> Expression {
        let mut left = self.parse_factor();

        while let Some(token) = self.tokens.peek() {
            match token {
                Token::Add => {
                    self.tokens.next();
                    let right = self.parse_factor();
                    left = Expression::BinaryOp {
                        left: Box::new(left),
                        right: Box::new(right),
                        op: Operation::Add,
                    }
                }
                Token::Substract => {
                    self.tokens.next();
                    let right = self.parse_factor();
                    left = Expression::BinaryOp {
                        left: Box::new(left),
                        right: Box::new(right),
                        op: Operation::Substract,
                    }
                }
                Token::Multiply => {
                    self.tokens.next();
                    let right = self.parse_factor();
                    left = Expression::BinaryOp {
                        left: Box::new(left),
                        right: Box::new(right),
                        op: Operation::Multiply,
                    }
                }
                Token::Divide => {
                    self.tokens.next();
                    let right = self.parse_factor();
                    left = Expression::BinaryOp {
                        left: Box::new(left),
                        right: Box::new(right),
                        op: Operation::Divide,
                    }
                }
                Token::Modulus => {
                    self.tokens.next();
                    let right = self.parse_factor();
                    left = Expression::BinaryOp {
                        left: Box::new(left),
                        right: Box::new(right),
                        op: Operation::Modulus,
                    }
                }
                _ => {
                    break;
                }
            }
        }
        left
    }

    fn parse_factor(&mut self) -> Expression {
        let left = self.parse_primary();
        left
    }

    fn parse_primary(&mut self) -> Expression {
        if let Some(Token::Number(n)) = self.tokens.next() {
            Expression::Number(*n)
        } else {
            panic!("Unexpected Token!");
        }
    }
}
