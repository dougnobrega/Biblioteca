// FFT
//
// Exemplos na main
//
// Soma O(n) & Multiplicacao O(nlogn)

template<typename T> void fft(vector<T> &a, bool f, int N, vector<int> &rev){
	for (int i = 0; i < N; i++)
		if (i < rev[i])
			swap(a[i], a[rev[i]]);
	int l, r, m;
	vector<T> roots(N);
	for (int n = 2; n <= N; n *= 2){
		T root = T::rt(f, n, N);
		roots[0] = 1;
		for (int i = 1; i < n/2; i++)
			roots[i] = roots[i-1]*root;
		for (int pos = 0; pos < N; pos += n){
			l = pos+0, r = pos+n/2, m = 0;
			while (m < n/2){
				auto t = roots[m]*a[r];
				a[r] = a[l] - t;
				a[l] = a[l] + t;
				l++; r++; m++;
			}
		}
	}
	if (f) {
		auto invN = T(1)/N;
		for(int i = 0; i < N; i++) a[i] = a[i]*invN;
	}
}


template<typename T> struct poly : vector<T> {
	poly(const vector<int> &coef):vector<T>(coef.size()){
		for (int i = 0; i < coef.size(); i++) this->at(i) = coef[i];
	}
	poly(const vector<T> &coef):vector<T>(coef){}
	poly(unsigned size, T val = 0):vector<T>(size, val){}
	poly(){}
	T operator()(T x){
		T ans = 0, curr_x(1);
		for (auto c : *this) {
			ans = ans+c*curr_x;
			curr_x = curr_x*x;

		}
		return ans;
	}
	poly<T> operator+(const poly<T> &r){
		const poly<T> &l = *this;
		int sz = max(l.size(), r.size());
		poly<T> ans(sz);
		for (unsigned i = 0; i < l.size(); i++)
			ans[i] = ans[i]+l[i];
		for (unsigned i = 0; i < r.size(); i++)
			ans[i] = ans[i]+r[i];
		return ans;
	}
	poly<T> operator-(poly<T> &r){
		for (auto &it : r) it = -it;
		return (*this)+r;
	}
	poly<T> operator*(const poly<T> r){
		const poly<T> &l = *this;
		int ln = l.size(), rn = r.size();
		int N = ln+rn+1;
		int log_n = T::fft_len(N);
		int n = 1 << log_n;
		vector<int> rev(n);

		for (int i = 0; i < n; ++i){
			rev[i] = 0;
			for (int j = 0; j < log_n; ++j)
				if (i & (1<<j))
					rev[i] |= 1 << (log_n-1-j);
		}
		if (N > n) throw logic_error("resulting poly to big");
		vector<T> X(n), Y(n);

		for (int i = 0; i < ln; i++) X[i] = l[i];
		for (int i = ln; i < n; i++) X[i] = 0;
		for (int i = 0; i < rn; i++) Y[i] = r[i];
		for (int i = rn; i < n; i++) Y[i] = 0;

		fft(X, false, n, rev);
		fft(Y, false, n, rev);

		for (int i = 0; i < n; i++)
			Y[i] = X[i]*Y[i];

		fft(Y, true, n, rev);
		poly<T> ans(N);
		for (int i = 0; i < N; i++)
			ans[i] = Y[i];

		//	while (!ans.empty() && ans.back() == 0)
		//		ans.pop_back();
		return ans;
	}

	pair<poly<T>, T> briot_ruffini(T r){//for p = Q*(x - r) + R, returns (Q, R)
		const poly<T> &l = *this;
		int sz = l.size();
		if (sz == 0) return {poly<T>(0), 0};
		poly<T> q(sz - 1);
		q.back() = l.back();
		for (int i = q.size()-2; i >= 0; i--){
			cout << i << "~" << q.size() << endl;
			q[i] = q[i+1]*r + l[i+1];
		}
		return {q, q[0]*r + l[0]};
	}
	friend ostream& operator<<(ostream &out, const poly<T> &p){
		if (p.empty()) return out;
		out << p.at(0);
		for (int i = 1; i < p.size(); i++)
			out << " + " << p.at(i) << "x^" << i;
		out << endl;
		return out;
	}
};

mt19937 rng((int) chrono::steady_clock::now().time_since_epoch().count());

const int MOD = 998244353;

using mint = mod_int<MOD>;

int main(){
	uniform_int_distribution<int> uid(0, MOD-1);
	int n = (1 << mint::fft_len()/2);
	auto rand_vec = [&](){
		vector<int> rd(n);
		for (int &i : rd) i = uid(rng);
		return rd;
	};

	poly<mint> p = rand_vec();
	poly<mint> q = rand_vec();
	poly<mint> sum = p+q;
	poly<mint> mult = p*q; 
	for (int i = 1; i <= 5000; i++){
		int x = uid(rng);
		auto P = p(x), Q = q(x), M = mult(x);
		if (P*Q != M) throw logic_error("bad implementation :(");
	}
	cout << "sucesso!" << endl;
	exit(0);

	exit(0);
}
