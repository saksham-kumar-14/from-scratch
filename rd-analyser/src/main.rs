use std::{collections::VecDeque, vec};

// represts a single node in CFG
struct Block {
    id: usize,
    genn: u64,
    kill: u64,
    succ: Vec<usize>,
    pre: Vec<usize>,
}

impl Block {
    fn new(id: usize, genn: u64, kill: u64) -> Self {
        Block {
            id: id,
            genn: genn,
            kill: kill,
            succ: Vec::new(),
            pre: Vec::new(),
        }
    }
}

// computes OUT given IN of a state
#[inline(always)]
fn transfer_function(in_state: u64, genn: u64, kill: u64) -> u64 {
    (in_state & !kill) | genn
}

// implementing MFP - Maximum Fixed point algorithm to solve, but ill apply MOP in exam
fn reaching_definitions(blocks: &[Block]) -> (Vec<u64>, Vec<u64>) {
    let n = blocks.len();

    // init in and out states as empty
    let mut in_states = vec![0u64; n];
    let mut out_states = vec![0u64; n];

    // track nodes which need to be re-evaluated
    let mut reval = VecDeque::new();
    let mut in_reval = vec![false; n];

    // first put all nodes in reval
    for i in 0..n {
        reval.push_back(i);
        in_reval[i] = true;
    }

    while let Some(u) = reval.pop_front() {
        in_reval[u] = false;
        let block = &blocks[u];

        // IN state = Union(OUT of all predecessors)
        let mut new_in = 0u64;
        for &p in &block.pre {
            new_in |= out_states[p];
        }
        in_states[u] = new_in;

        // OUT state using transfer function
        let new_out = transfer_function(new_in, block.genn, block.kill);

        // if OUT changes, add successors to the worklist
        if new_out != out_states[u] {
            out_states[u] = new_out;

            for &succ in &block.succ {
                if !in_reval[succ] {
                    reval.push_back(succ);
                    in_reval[succ] = true;
                }
            }
        }
    }
    return (in_states, out_states);
}

fn main() {
    let mut b0 = Block::new(0, 0b0001, 0b0010); // Gens 0, Kills 1
    let mut b1 = Block::new(1, 0b0010, 0b0001); // Gens 1, Kills 0
    let mut b2 = Block::new(2, 0b0100, 0b1000); // Gens 2, Kills 3
    let mut b3 = Block::new(3, 0b1000, 0b0100); // Gens 3, Kills 2

    //b0 -> b1, b0 -> b2, b1 -> b3, b2 -> b3
    b0.succ = vec![1, 2];
    b1.pre = vec![0];
    b1.succ = vec![3];
    b2.pre = vec![0];
    b2.succ = vec![3];
    b3.pre = vec![1, 2];

    let cfg = vec![b0, b1, b2, b3];

    let (in_states, out_states) = reaching_definitions(&cfg);

    for i in 0..cfg.len() {
        println!("Block {}", i);
        println!("IN:  {:04b}", in_states[i]);
        println!("OUT: {:04b}", out_states[i]);
    }
}
