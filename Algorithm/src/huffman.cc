#include <string>
#include <map>
#include <queue>
#include <bitset>
#include <cstdio>
#include <cstdlib>
#include <cstring>
static const int LEN = 10240;
static const int NCH = 130;

int weight[NCH];

struct node_t {
	bool is_leaf;
	int left, right;
	char val;
	int weight;
	int id;

	bool operator < (const struct node_t& t) const {
		return weight > t.weight;
	}
};

struct node_t nodes[NCH * 2 + 1];
std::priority_queue<struct node_t> pq;
std::map<char, std::string> huffman_mp;
std::map<std::string, char> endcode_mp;
std::map<char, std::string> decode_mp;

void inline init_encode()
{
	for (char c = 0x80; c < 127; c++) {
		std::bitset<8> bits(c);
		std::string s = bits.to_string();
		endcode_mp[s] = c;
		decode_mp[c] = s;
	}
	char c = 127;
	std::bitset<8> bits(c);
	std::string s = bits.to_string();
	endcode_mp[s] = c;
	decode_mp[c] = s;
}

char trace[NCH];
void search_encode(int root, int len)
{
	if (nodes[root].is_leaf) {
		trace[len] = '\0';
		huffman_mp[nodes[root].val] = std::string(trace);
	}
	else {
		trace[len] = '0';
		search_encode(nodes[root].left, len + 1);

		trace[len] = '1';
		search_encode(nodes[root].right, len + 1);
	}
}

void bit_encode(std::string& str, std::string& out)
{
	char *cstr = const_cast<char *>(str.c_str());
	int len = str.size();
	for (int i = 0; i < len / 8; i++) {
		char tmp = cstr[8 * i + 8];
		cstr[8 * i + 8] = '\0';
		char ch = endcode_mp[cstr + 8 * i];
		cstr[8 * i + 8] = tmp;
		out.push_back(ch);
	}
}

int huffman_encode(FILE *fp_in, FILE *fp_out)
{
	int ncount = 1;
	for (int i = 0; i < NCH; i++) {
		if (weight[i]) {
			nodes[ncount].is_leaf = true;
			nodes[ncount].left = 0;
			nodes[ncount].right = 0;
			nodes[ncount].val = i;
			nodes[ncount].id = ncount;
			nodes[ncount].weight = weight[i];
			pq.push(nodes[ncount]);
			++ncount;
		}
	}

	while (pq.size() != 1) {
		struct node_t node1 = pq.top();
		pq.pop();

		struct node_t node2 = pq.top();
		pq.pop();

		nodes[ncount].is_leaf = false;
		nodes[ncount].left = node1.id;
		nodes[ncount].right = node2.id;
		nodes[ncount].weight = node1.weight + node2.weight;
		nodes[ncount].id = ncount;
		pq.push(nodes[ncount]);

		++ncount;
	}

	struct node_t node = pq.top();
	pq.pop();

	int root = node.id;

	search_encode(root, 0);

	fseek(fp_in, 0, SEEK_SET);

	int idx = 0;
	char str[LEN];
	std::string out;
	std::string buffer;
	while (fgets(str, LEN, fp_in) != NULL) {
		int len = strlen(str);
		for (int i = 0; i < len; i++) {
			buffer.append(huffman_mp[str[i]]);
		}
		bit_encode(buffer, out);
		char *t = const_cast<char *>(out.c_str());
		fwrite(t, sizeof(char), buffer.size() / 8, fp_out);

		//		fprintf(fp_out, "%s", out.c_str());
		//		printf("buffer %s\n", buffer.c_str());
		//		printf("out %zu\n", out.size());

		out = "";
		len = buffer.size();
		buffer.erase(buffer.begin(), buffer.begin() + len / 8 * 8);
	}
	if (buffer.size()) {
		char ch = endcode_mp[buffer];
		fwrite(&ch, sizeof(char), 1, fp_out);
		ch = buffer.size();
		fprintf(fp_out, "%c", ch);
	}

	return root;
}

int cur_idx = 0;
void inline search_decode(int root, char val, FILE *fp)
{
	if (val == '0') {
		cur_idx = nodes[nodes[cur_idx].left].id;
	}
	else {
		cur_idx = nodes[nodes[cur_idx].right].id;
	}

	if (nodes[cur_idx].is_leaf) {
		fprintf(fp, "%c", nodes[cur_idx].val);
		cur_idx = root;
	}
}

void bit_decode(std::string& in, char *str, int len)
{
	for (int i = 0; i < len; i++) {
		in.append(decode_mp[str[i]]);
	}
}

void huffman_decode(int root, FILE *fp_in, FILE *fp_out)
{

	cur_idx = root;
	std::string in;
	char str[LEN + 1]; 
	int res = 0;
	fseek(fp_in, 0, SEEK_END);
	int total = ftell(fp_in);
	int cur = 0;
	fseek(fp_in, 0, SEEK_SET);
	while (true) {
		res = fread(str, sizeof(char), LEN, fp_in);
		if (res != LEN)
			break;

		bit_decode(in, str, LEN);

		cur += LEN;
		int len = in.size();
		if (cur == total) {
			int num = str[res - 1];
			int i = 0;
			for (; i < len - 16; i++) {//last byte stores the size of the previous byte
				search_decode(root, in[i], fp_out);
			}
			i += num;
			for (; i < 8; i++) {
				search_decode(root, in[i], fp_out);
			}
		}
		for (int i = 0; i < len; i++) {
			search_decode(root, in[i], fp_out);
		}
		in = "";
	}

	if (feof(fp_in)) {

	}
	else {
		printf("error!\n");
	}

	if (res != 0) {
		int num = str[res - 1];
		bit_decode(in, str, res);
		int len = in.size();
		int i = 0;
		for (; i < len - 16; i++) {
			search_decode(root, in[i], fp_out);
		}
		i += num;
		for (; i < 8; i++) {
			search_decode(root, in[i], fp_out);
		}
	}
}

void buffer_sep(FILE *fp)
{
	char str[LEN];
	while (fgets(str, LEN, fp) != NULL) {
		//printf("str %s\n", str);
		int len = strlen(str);
		for (int i = 0; i < len; i++) {
			weight[str[i]]++;
		}
	}
}

int main()
{
	FILE *fp_in = fopen("../data/graph.txt", "r+");

	if (fp_in == NULL) {
		fprintf(stderr, "Open file aesop_fables error!");
		exit(1);
	}

	buffer_sep(fp_in);

	FILE *fp_encode = fopen("../output/graph_encode.txt", "wb+");

	if (fp_encode == NULL) {
		fprintf(stderr, "Writing file aesop_fables_encode_encode error!");
		exit(1);
	}

	init_encode();

	int root = huffman_encode(fp_in, fp_encode);

	FILE *fp_decode = fopen("../output/graph_decode.txt", "w+");

	if (fp_decode == NULL) {
		fprintf(stderr, "Writing file aesop_fables_decode error!");
		exit(1);
	}

	huffman_decode(root, fp_encode, fp_decode);

	fclose(fp_in); fclose(fp_decode);
	fclose(fp_encode);
	return 0;
}
