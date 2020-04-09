// DFAtoREGEX.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <set>
#include <algorithm> 
using namespace std;

ifstream f("dfa.txt");

/*
number of states
states (Q)
number of characters
each character (Σ or Sigma)
number of transitions
transitions (state character state) (δ or delta)
initial state (q0)
number of final states
final states (F)
*/


int findMax(set<int> my_set)
{
	int max_element;
	if (!my_set.empty())
	{
		max_element = *(my_set.rbegin());
		return max_element;
	}
	return 0;
}

int findMin(set<int> my_set)
{
	int min_element;
	if (!my_set.empty()) {
		min_element = *my_set.begin();
		return min_element;
	}
	return 0;
}

void readDFA(int& nrStates, set<int>& Q, int& nrLetters, set<char>& Sigma, int& nrTransitions, map<pair<int, string>, int>& delta, int& q0, int& nrFinalStates, set<int>& F) {
	f >> nrStates;
	for (int i = 0; i < nrStates; ++i)
	{
		int q;
		f >> q;
		Q.insert(q);
	}

	f >> nrLetters;
	for (int i = 0; i < nrLetters; ++i)
	{
		char ch;
		f >> ch;
		Sigma.insert(ch);
	}

	f >> nrTransitions;
	for (int i = 0; i < nrTransitions; ++i)
	{
		int s, d;
		string ch;
		f >> s >> ch >> d;
		delta[{s, ch}] = d;
	}

	f >> q0;

	f >> nrFinalStates;
	for (int i = 0; i < nrFinalStates; ++i)
	{
		int q;
		f >> q;
		F.insert(q);
	}
}

void printDFA(int& nrStates, set<int>& Q, int& nrLetters, set<char>& Sigma, int& nrTransitions, map<pair<int, string>, int>& delta, int& q0, int& nrFinalStates, set<int>& F) {
	cout << nrStates << endl;
	for (int stare : Q)
		cout << stare << " ";
	cout << endl;
	cout << nrLetters << endl;
	for (char litera : Sigma)
		cout << litera << " ";
	cout << endl;
	cout << nrTransitions << endl;
	map<pair<int, string>, int>::iterator it = delta.begin();
	while (it != delta.end()) {
		pair<int, string> cheie = it->first;
		int stare1 = cheie.first;
		string litera = cheie.second;
		int stare2 = it->second;
		cout << stare1 << " " << litera << " " << stare2 << endl;
		it++;
	}
	cout << q0 << endl;
	cout << nrFinalStates << endl;
	for (int final : F)
		cout << final << " ";
	cout << endl;
}

void modifyInitialState(int& nrStates, set<int>& Q, int& nrLetters, set<char>& Sigma, int& nrTransitions, map<pair<int, string>, int>& delta, int& q0) {

	//daca exista tranzitii care intra in starea initiala, se adauga o noua stare
	//initiala qi si o lambda-tranzitie de la qi la vechea stare initiala
	map<pair<int, string>, int>::iterator it = delta.begin();
	while (it != delta.end()) {
		pair<int, string> cheie = it->first;
		int stare1 = cheie.first;
		string litera = cheie.second;
		int stare2 = it->second;
		if (stare2 == q0) {
			int minim = findMin(Q);
			Q.insert(minim - 1);
			delta[{-1, "."}] = q0;
			Sigma.insert('.');
			nrTransitions++;
			nrStates++;
			nrLetters++;
			q0 = -1;
			break;
		}
		it++;
	}
}

void modifyFinalState(int& nrStates, set<int>& Q, int& nrLetters, set<char>& Sigma, int& nrTransitions, map<pair<int, string>, int>& delta, int& nrFinalStates, set<int>& F) {

	// cautam daca exista vreo tranzitie catre vreo stare finala
	int nuexista = 1; // presupunem ca nu exsita
	map<pair<int, string>, int>::iterator it = delta.begin();
	while (it != delta.end()) {
		int destinatie = it->second;
		if (F.find(destinatie) != F.end()) { // daca destinatie e stare finala
			nuexista = 0; // deci exista tranzitie catre o stare finala
			break;
		}
		it++;
	}

	// daca sunt mai multe stari finale sau exista tranzitii catre o stare finala, 
	// facem o noua stare si o legam de vechile stari finale prin lambda tranzitii
	if (nrFinalStates > 1 && nuexista == 0) {
		nrStates++;
		int maxim = findMax(Q);
		Q.insert(maxim + 1);
		for (int final : F) {
			delta[{final, "."}] = maxim + 1;
			nrTransitions++;
			if (Sigma.find('.') == Sigma.end()) { // daca nu exista lambda in sigma
				nrLetters++;
				Sigma.insert('.');
			}
		}
		F.clear();
		F.insert(maxim + 1);
		nrFinalStates = 1;
	}
}

string isTransition(int a, int b, map<pair<int, string>, int>& delta) {

	map<pair<int, string>, int>::iterator it = delta.begin();
	while (it != delta.end()) {
		if (it->first.first == a && it->second == b)
			return it->first.second;
		it++;
	}
	return "";
}


void removeState(int stare, int& nrStates, set<int>& Q, int& nrTransitions, map<pair<int, string>, int>& delta) {
	string expresie;
	set<int> in, out;
	map<pair<int, string>, int>::iterator it = delta.begin();
	while (it != delta.end()) {
		pair<int, string> cheie = it->first;
		int stare1 = cheie.first;
		int stare2 = it->second;
		if (stare1 == stare && stare2 != stare) out.insert(stare2);
		if (stare2 == stare && stare1 != stare) in.insert(stare1);
		it++;
	}
	for (int intrare : in) {
		for (int iesire : out) {
			//produs cartezian
			/*
			Presupunem că vrem să eliminăm starea qk şi că există etichetele (qi,qk), (qk,qj) şi eventual bucla
			(qk, qk).Atunci obţinem noua etichetă între stările qi şi qj reunind[(fosta etichetă directă de la qi la
			qj), sau nimic(∅) dacă nu există drum direct] cu[(eticheta de la qi la qk) concatenată cu(stelarea
			etichetei buclei de la qk la qk, sau λ dacă bucla nu există) concatenată cu(eticheta de la qk la qj)].
			*/
			if (isTransition(intrare, iesire, delta) != "") {
				string expresie = isTransition(intrare, iesire, delta);
				expresie += " + (" + isTransition(intrare, stare, delta) + ") ( . +" + isTransition(stare, stare, delta) + ")* (" + isTransition(stare, iesire, delta) + ")";
				int	nr = 0;
				for (char c : expresie) {
					//inlocuim punct cu lambda
					if (c == '.') expresie.replace(nr, 1, "lambda");
					nr++;
				}
				for (map<pair<int, string>, int>::iterator it2 = delta.begin(); it2 != delta.end();) {
					if (it2->first.first == intrare && it2->second == iesire)
					{
						//stergem toate tranzitiile intrare->iesire pentru a o adauga pe cea noua
						it2 = delta.erase(it2);
						nrTransitions--;
					}
					else it2++;
				}

				delta[{intrare, expresie}] = iesire;
			}
			else {
				string expresie = "(" + isTransition(intrare, stare, delta) + ") ( . +" + isTransition(stare, stare, delta) + ")* (" + isTransition(stare, iesire, delta) + ")";
				int	nr = 0;
				for (char c : expresie) {
					//inlocuim punct cu lambda
					if (c == '.') expresie.replace(nr, 1, "lambda");
					nr++;
				}
				delta[{intrare, expresie}] = iesire;

			}
			for (map<pair<int, string>, int>::iterator it2 = delta.begin(); it2 != delta.end();) {
				if (it2->first.first == stare || it2->second == stare)
				{
					//stergem toate tranzitiile care contin starea
					it2 = delta.erase(it2);
					nrTransitions--;
				}
				else it2++;
			}
			nrStates--;
		}
	}
}

void removeStates(int& nrStates, set<int>& Q, int& nrTransitions, map<pair<int, string>, int>& delta) {
	int initial = findMin(Q);
	int final = findMax(Q);
	for (int i : Q) {
		if (i != initial && i != final) removeState(i, nrStates, Q, nrTransitions, delta);
	}
	cout << delta.begin()->first.second;
}

int main()
{
	// M = (Q,Σ,δ,q0,F)
	// the symbol for lambda is .

#pragma region declars

	set<int> Q, F;
	set<char> Sigma;
	int q0;
	map<pair<int, string>, int> delta;

	int noOfStates;
	int noOfLetters;
	int noOfTransitions;
	int noOfFinalStates;

#pragma endregion

	readDFA(noOfStates, Q, noOfLetters, Sigma, noOfTransitions, delta, q0, noOfFinalStates, F);

	modifyInitialState(noOfStates, Q, noOfLetters, Sigma, noOfTransitions, delta, q0);

	modifyFinalState(noOfStates, Q, noOfLetters, Sigma, noOfTransitions, delta, noOfFinalStates, F);

	printDFA(noOfStates, Q, noOfLetters, Sigma, noOfTransitions, delta, q0, noOfFinalStates, F);

	cout << "-------------------------------------" << endl;

	removeStates(noOfStates, Q, noOfTransitions, delta);

	return 0;
}
