#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <bitset>

using namespace std;

const int INF = 1'000'000'000;
const int NOTHING = 2 * INF;

vector<int> weights = {0, 1, 10, 1000, INF};
vector<int> free_row = {6, 6, 6, 6, 6, 6, 6};

unordered_map<unsigned long long, bool> used;
unordered_map<unsigned long long, int> mem;

unordered_map<unsigned long long, bool> eval_used;
unordered_map<unsigned long long, int> eval_mem;

string str(bitset<49> &pos) {
	string ans = "";
	for (int i = 1; i < 7; ++i) {
		for (int j = 0; j < 7; ++j) {
			if (i <= free_row[j]) {
				ans += '-';
			}
			else if (!pos[7 * i + j]) {
				ans += 'x';
			}
			else if (pos[7 * i + j]) {
				ans += 'o';
			}
			else {
				ans += '-';
			}
		}
	}
	return ans;
}

void visualize(string &pos) {
	for (int i = 0; i < 6; ++i) {
		for (int j = 0; j < 7; ++j) {
			cout << pos[7 * i + j];
		}
		cout << '\n';
	}
}

int clamp(int x) {
	return min(max(x, -INF), INF);
}

int cost(bitset<49> &pos, int i, int j) {
    if (free_row[j] >= i) {
        return 0;
    }
	return (pos[i * 7 + j] ? -1 : 1);
}

int dominate(int a, int b, int c, int d) {
    return abs(min(min(a, b), min(c, d)) - max(max(a, b), max(c, d))) < 2;
}

int sign(int x) {
	if (x > 0) {
		return 1;
	}
	else if (x < 0) {
		return -1;
	}
	else {
		return 0;
	}
}

int eval(bitset<49> &pos, bool turn) {
    if (eval_used[pos.to_ullong()]) {
        return eval_mem[pos.to_ullong()];
    }
	vector<int> cnt = {0, 0, 0, 0, 0};
	for (int i = 1; i < 7; ++i) {
		for (int j = 0; j < 7; ++j) {
			if (i < 4) {
				cnt[abs(cost(pos, i, j) + cost(pos, i + 1, j) + cost(pos, i + 2, j) + cost(pos, i + 3, j))] += (turn ? -1 : 1) * sign(cost(pos, i, j) + cost(pos, i + 1, j) + cost(pos, i + 2, j) + cost(pos, i + 3, j));
			}
			if (j < 4) {
				cnt[abs(cost(pos, i, j) + cost(pos, i, j + 1) + cost(pos, i, j + 2) + cost(pos, i, j + 3))] += (turn ? -1 : 1) * sign(cost(pos, i, j) + cost(pos, i, j + 1) + cost(pos, i, j + 2) + cost(pos, i, j + 3));
			}
			if (i < 4 && j < 4) {
				cnt[abs(cost(pos, i, j) + cost(pos, i + 1, j + 1) + cost(pos, i + 2, j + 2) + cost(pos, i + 3, j + 3))] += (turn ? -1 : 1) * sign(cost(pos, i, j) + cost(pos, i + 1, j + 1) + cost(pos, i + 2, j + 2) + cost(pos, i + 3, j + 3));
			}
			if (i < 4 && j > 2) {
				cnt[abs(cost(pos, i, j) + cost(pos, i + 1, j - 1) + cost(pos, i + 2, j - 2) + cost(pos, i + 3, j - 3))] += (turn ? -1 : 1) * sign(cost(pos, i, j) + cost(pos, i + 1, j - 1) + cost(pos, i + 2, j - 2) + cost(pos, i + 3, j - 3));
			}
		}
	}
	int val = 0;
	for (int i = 0; i < 5; ++i) {
		val += cnt[i] * weights[i];
	}
	if (cnt[4] != 0) {
        val = sign(cnt[4]) * weights[4];
	}
	val = clamp(val);
	eval_used[pos.to_ullong()] = true;
	eval_mem[pos.to_ullong()] = val;
	return val;
}

void push(bitset<49> &pos, bool turn, int col) {
	pos.set(7 * (free_row[col]) + col, turn);
	pos.set(7 * (free_row[col] - 1) + col, 1);
	free_row[col]--;
}

void unpush(bitset<49> &pos, int col) {
	free_row[col]++;
	pos.set(7 * (free_row[col]) + col, 1);
	pos.set(7 * (free_row[col] - 1) + col, 0);
}

int search(bitset<49> &pos, bool turn, int depth_left, int alpha) {
	if (used[pos.to_ullong()] == 1) {
		return mem[pos.to_ullong()];
	}
	int evaluation = eval(pos, turn);
	if (depth_left <= 0 || abs(evaluation) == INF) {
		return evaluation;
	}
    vector<int> order = {0, 1, 2, 3, 4, 5, 6};
	random_shuffle(order.begin(), order.end());
	int best_eval = -INF;
	bool flag = true;
	for (int col = 0; col < 7; ++col) {
		if (free_row[order[col]] < 1) {
			continue;
		}
		push(pos, turn, order[col]);
		best_eval = max(best_eval, -search(pos, !turn, depth_left - 1, -best_eval));
		unpush(pos, order[col]);
		if (best_eval > alpha) {
            flag = false;
            break;
		}
	}

    if (flag) {
        used[pos.to_ullong()] = 1;
        mem[pos.to_ullong()] = best_eval;
    }

	return best_eval;
}

vector<int> get_moves_eval(bitset<49> &pos, bool turn, int depth) {
	vector<int> evals(7, NOTHING);
	for (int col = 0; col < 7; ++col) {
		if (free_row[col] < 1) {
			continue;
		}
		push(pos, turn, col);
		evals[col] = -search(pos, !turn, depth, INF);
		unpush(pos, col);
	}
	return evals;
}

int get_move(bitset<49> &pos, bool turn, int depth) {
    mem.clear();
    used.clear();
	vector<int> evals = get_moves_eval(pos, turn, depth);
	int res = -1;
	for (int col = 0; col < 7; ++col) {
		if (evals[col] != NOTHING && (res == -1 || evals[res] < evals[col])) {
			res = col;
		}
	}
	return res;
}

signed main() {
    cout << "Choose your side to play (w/b): ";
    string side; cin >> side;
    bool ms = !(side == "b");
    bitset<49> pos{"1111111000000000000000000000000000000000000000000"};
	cout << "Get ready...\n";
	int ply = 0;
	int lcol = 0;
	while (ply < 42 && abs(eval(pos, 0)) < INF) {
		if (ply % 2 == ms) {
			string hsh = str(pos);
			system("cls");
			visualize(hsh);
			cout << "Let me think...\n";
			int col = get_move(pos, ms, 7);
			lcol = col + 1;
			push(pos, ms, col);
		}
		else {
			string hsh = str(pos);
			system("cls");
			visualize(hsh);
			cout << lcol << ' ' << mem[pos.to_ullong()] << endl;
			cout << "Enter your move: ";
			int col; cin >> col;
			push(pos, !ms, col - 1);
		}
		ply += 1;
	}
    string hsh = str(pos);
    system("cls");
    visualize(hsh);
}
