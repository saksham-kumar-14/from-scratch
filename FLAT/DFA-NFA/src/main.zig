const std = @import("std");

const states = u32;
const transitions = [][]u32;

const DFA = struct {
    n: u32,
    m: u32,
    S: u32,
    F: states,
    D: transitions,
};

const NFA = struct {
    n: u32,
    m: u32,
    S: u32,
    F: u32,
    D: transitions,
};

fn readNFA(filename: []const u8, allocator: std.mem.Allocator) !NFA {
    var file = try std.fs.cwd().openFile(filename, .{});
    defer file.close();

    var buf_reader = std.io.bufferedReader(file.reader());
    var in_stream = buf_reader.reader();
    var buf: [1024]u8 = undefined;

    var current: u8 = 'n';
    var N: NFA = NFA{
        .n = 0,
        .m = 0,
        .S = 0,
        .F = 0,
        .D = undefined,
    };

    while (try in_stream.readUntilDelimiterOrEof(&buf, '\n')) |line| {
        if (current == 'n') {
            const trimmed = std.mem.trim(u8, line, " \r\t");
            if (trimmed.len == 0) continue;
            const n = try std.fmt.parseInt(u32, trimmed, 10);
            N.n = n;
            current = 'm';
            N.D = try allocator.alloc([]u32, n);
        } else if (current == 'm') {
            const trimmed = std.mem.trim(u8, line, " \r\t");
            if (trimmed.len == 0) continue;
            const m = try std.fmt.parseInt(u32, trimmed, 10);
            N.m = m;
            for (0..N.n) |i| {
                N.D[i] = try allocator.alloc(u32, m);
                @memset(N.D[i], 0);
            }
            current = 's';
        } else if (current == 's') {
            var it = std.mem.tokenizeAny(u8, line, " \r\t");
            while (it.next()) |pt| {
                const p = try std.fmt.parseInt(i32, pt, 10);
                if (p == -1) {
                    current = 'f';
                } else {
                    N.S |= (@as(u32, 1) << @as(u5, @intCast(p)));
                }
            }
        } else if (current == 'f') {
            var it = std.mem.tokenizeAny(u8, line, " \r\t");
            while (it.next()) |pt| {
                const p = try std.fmt.parseInt(i32, pt, 10);
                if (p == -1) {
                    current = 'd';
                } else {
                    N.F |= (@as(u32, 1) << @as(u5, @intCast(p)));
                }
            }
        } else if (current == 'd') {
            var it = std.mem.tokenizeAny(u8, line, " \r\t");
            if (it.peek() == null) continue;

            const pi_str = it.next() orelse continue;
            const pi = try std.fmt.parseInt(i32, pi_str, 10);

            if (pi == -1) {
                break;
            }

            const j_str = it.next() orelse continue;
            const k_str = it.next() orelse continue;

            const i: usize = @intCast(pi);
            const j = try std.fmt.parseInt(usize, j_str, 10);
            const k = try std.fmt.parseInt(u5, k_str, 10);

            N.D[i][j] |= (@as(u32, 1) << k);
        }
    }
    return N;
}

fn print_states(S: states, n: u32) void {
    var q: u32 = 0;
    std.debug.print("{{", .{});
    var first = true;
    while (q < n) : (q += 1) {
        if ((S & (@as(u32, 1) << @as(u5, @intCast(q)))) != 0) {
            if (!first) std.debug.print(", ", .{});
            std.debug.print("{d}", .{q});
            first = false;
        }
    }
    std.debug.print("}}", .{});
}

fn print_NFA(N: NFA) void {
    std.debug.print("Number of States: {d}\n", .{N.n});
    std.debug.print("Input Alphabets: ", .{});
    var alph: u32 = 0;
    while (alph < N.m) : (alph += 1) {
        std.debug.print("{d}, ", .{alph});
    }
    std.debug.print("\n", .{});
    std.debug.print("Start states: ", .{});
    print_states(N.S, N.n);
    std.debug.print("\nFinal states: ", .{});
    print_states(N.F, N.n);
    std.debug.print("\nTransition states:\n", .{});
    for (0..N.n) |i| {
        for (0..N.m) |j| {
            std.debug.print("Delta({d}, {d}) ", .{ i, j });
            print_states(N.D[i][j], N.n);
            std.debug.print("\n", .{});
        }
    }
}

fn freeNFA(nfa: NFA, allocator: std.mem.Allocator) void {
    for (nfa.D) |row| {
        allocator.free(row);
    }
    allocator.free(nfa.D);
}

pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    const stdin = std.io.getStdIn().reader();
    const stdout = std.io.getStdOut().writer();

    try stdout.print("Enter file to read from: ", .{});

    var input_buffer: [1024]u8 = undefined;
    if (try stdin.readUntilDelimiterOrEof(&input_buffer, '\n')) |user_input| {
        const filename = std.mem.trim(u8, user_input, " \r\t");
        const N = readNFA(filename, allocator) catch |err| {
            std.debug.print("Error reading NFA: {}\n", .{err});
            return;
        };
        defer freeNFA(N, allocator);

        std.debug.print("<< Input NFA >>\n", .{});
        print_NFA(N);
    } else {
        try stdout.print("No input received.\n", .{});
    }
}
