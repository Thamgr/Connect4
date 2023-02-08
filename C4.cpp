#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>

using namespace std;

const int INF = 1'000'000'000;
const int NOTHING = 2 * INF;

enum STATE {
	WHITE = 1,
	BLACK = -1,
	EMPTY = 0
};

vector<int> weights = {0, 1, 10, 1000, INF};
vector<int> free_row = {5, 5, 5, 5, 5, 5, 5};

unordered_map<string, int> used;
unordered_map<string, int> mem;

string str(vector<STATE> &pos) {
	string ans = "";
	for (int i = 0; i < 6; ++i) {
		for (int j = 0; j < 7; ++j) {
			if (pos[7 * i + j] == STATE::WHITE) {
				ans += 'x';
			}
			else if (pos[7 * i + j] == STATE::BLACK) {
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

STATE next(STATE turn) {
	if (turn == STATE::WHITE) {
		return STATE::BLACK;
	}
	else {
		return STATE::WHITE;
	}
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

int eval(vector<STATE> &pos, STATE turn) {
	vector<int> cnt = {0, 0, 0, 0, 0};
	for (int i = 0; i < 6; ++i) {
		for (int j = 0; j < 7; ++j) {
			if (i < 3) {
				cnt[abs(pos[7 * i + j] + pos[7 * (i + 1) + j] + pos[7 * (i + 2) + j] + pos[7 * (i + 3) + j])] += turn * sign(pos[7 * i + j] + pos[7 * (i + 1) + j] + pos[7 * (i + 2) + j] + pos[7 * (i + 3) + j]);
			}
			if (j < 4) {
				cnt[abs(pos[7 * i + j] + pos[7 * i + j + 1] + pos[7 * i + j + 2] + pos[7 * i + j + 3])] += turn * sign(pos[7 * i + j] + pos[7 * i + j + 1] + pos[7 * i + j + 2] + pos[7 * i + j + 3]);
			}
			if (i < 3 && j < 4) {
				cnt[abs(pos[7 * i + j] + pos[7 * (i + 1) + j + 1] + pos[7 * (i + 2) + j + 2] + pos[7 * (i + 3) + j + 3])] += turn * sign(pos[7 * i + j] + pos[7 * (i + 1) + j + 1] + pos[7 * (i + 2) + j + 2] + pos[7 * (i + 3) + j + 3]);
			}
			if (i < 3 && j > 2) {
				cnt[abs(pos[7 * i + j] + pos[7 * (i + 1) + j - 1] + pos[7 * (i + 2) + j - 2] + pos[7 * (i + 3) + j - 3])] += turn * sign(pos[7 * i + j] + pos[7 * (i + 1) + j - 1] + pos[7 * (i + 2) + j - 2] + pos[7 * (i + 3) + j - 3]);
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
	return val;
}

void push(vector<STATE> &pos, STATE turn, int col) {
	pos[7 * free_row[col] + col] = turn;
	free_row[col]--;
}

void unpush(vector<STATE> &pos, int col) {
	pos[7 * (free_row[col] + 1) + col] = STATE::EMPTY;
	free_row[col]++;
}

int search(vector<STATE> &pos, STATE turn, int depth_left, int alpha) {
	if (used[str(pos)] == 1) {
		return mem[str(pos)];
	}
	int evaluation = eval(pos, turn);
	if (depth_left <= 0 || abs(evaluation) > INF / 3) {
		return evaluation;
	}
    vector<int> order = {0, 1, 2, 3, 4, 5, 6};
	random_shuffle(order.begin(), order.end());
	int best_eval = -INF;
	for (int col = 0; col < 7; ++col) {
		if (free_row[order[col]] < 0) {
			continue;
		}
		push(pos, turn, order[col]);
		best_eval = max(best_eval, -search(pos, next(turn), depth_left - 1, -best_eval));
		unpush(pos, order[col]);
		if (best_eval > alpha) {
            break;
		}
	}

	used[str(pos)] = 1;
    mem[str(pos)] = best_eval;

	return best_eval;
}

vector<int> get_moves_eval(vector<STATE> &pos, STATE turn, int depth) {
	vector<int> evals(7, NOTHING);
	for (int col = 0; col < 7; ++col) {
		if (free_row[col] < 0) {
			continue;
		}
		push(pos, turn, col);
		evals[col] = -search(pos, next(turn), depth, INF);
		unpush(pos, col);
	}
	return evals;
}

int get_move(vector<STATE> &pos, STATE turn, int depth) {
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
    int ms = (side == "b" ? 0 : 1);
	vector<STATE> pos(42, STATE::EMPTY);
	vector<STATE> sd = {STATE::WHITE, STATE::BLACK};
	cout << "Get ready...\n";
	int ply = 0;
	int lcol = 0;
	while (ply < 42 && abs(eval(pos, STATE::WHITE)) < INF) {
		if (ply % 2 == ms) {
			string hsh = str(pos);
			system("cls");
			visualize(hsh);
			cout << "Let me think...\n";
			int col = get_move(pos, sd[ms], 7);
			lcol = col + 1;
			push(pos, sd[ms], col);
		}
		else {
			string hsh = str(pos);
			system("cls");
			visualize(hsh);
			cout << lcol << ' ' << mem[hsh] << endl;
			cout << "Enter your move: ";
			int col; cin >> col;
			push(pos, sd[1 - ms], col - 1);
		}
		ply += 1;
	}
    string hsh = str(pos);
    system("cls");
    visualize(hsh);
}
