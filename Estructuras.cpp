#include <iostream>
#include <string>
#include <string.h>
#include<fstream>
#include <cstdio>
#include <thread>
#include<fstream>
#include <chrono>
#include <mutex>
#include <conio.h>
#include <windows.h>
using namespace std;

class nodoB {
public:
	nodoB(int cod, int car, int cat, string nom) { codigo = cod; carrera = car; categoria = cat; nombre = nom; direccion = ""; telefono = ""; }
	nodoB(int cod, int car, string nom, string dir, string tel) { codigo = cod; carrera = car; nombre = nom; direccion = dir, telefono = tel; categoria = NULL; }
private:
	int codigo, carrera, categoria;
	string nombre, direccion, telefono;

	friend class ProfesoresEstudiantes;
	friend class Pagina;
};
typedef nodoB *pnodoB;

class Pagina;
typedef Pagina *pPagina;

class Pagina {
private:
	pnodoB *claves;
	pPagina *ramas;
	int cuenta;
	int m;
public:
	Pagina(int orden) {
		m = orden;
		claves = new pnodoB[orden];
		ramas = new pPagina[orden];
		for (int k = 0; k <= orden; k++)
			ramas[k] = NULL;
	}
	bool nodoLLeno() {
		return (cuenta == m - 1);
	}
	bool nodoSinLlenar() {
		return (cuenta < m / 2);
	}
	pnodoB getClave(int i) { return claves[i]; }
	void setClave(int i, pnodoB clave) { claves[i] = clave; }
	pPagina getRama(int i) { return ramas[i]; }
	void setRama(int i, pPagina p) { ramas[i] = p; }
	int getCuenta() { return cuenta; }
	void setCuenta(int valor) { cuenta = valor; }

	friend class ArbolCarreras;
};

class ProfesoresEstudiantes {
protected:
	int orden;
	pPagina raiz;
public:
	ProfesoresEstudiantes() {
		orden = 5;
		raiz = NULL;
	};
	bool arbolBvacio() { return raiz == NULL; }
	pPagina getRaiz() { return raiz; }
	void setRaiz(pPagina r) { raiz = r; }
	void crearNodoProf(int codigo, int carrera, int categoria, string nombre);
	void crearNodoEst(int codigo, int carrera, string nombre, string direccion, string telefono);
	bool buscar(pPagina actual, int cl, int &k);
	pnodoB buscarN(pPagina actual, int cl, int &k);
	int buscarCarrera(int cl);
	int buscarCarrera(pPagina actual, int cl, int &k);
	bool buscarNodo(pPagina actual, int cl, int &k);
	bool buscar(int cl);
	void insertar(pnodoB cl);
	void insertar(pPagina raiz, pnodoB cl);
	bool empujar(pPagina actual, pnodoB cl, pnodoB &mediana, pPagina &k);
	void meterPagina(pPagina actual, pnodoB cl, pPagina ramaDr, int k);
	void dividirNodo(pPagina actual, pnodoB &mediana, pPagina &k, int pos);
	void leerProfesores(string txt);
	void leerEstudiantes(string txt);
	void escribir();
	void escribir(pPagina r);

	friend class ArbolCarreras;
};
typedef ProfesoresEstudiantes *pProfesoresEstudiantes;

void ProfesoresEstudiantes::crearNodoEst(int codigo, int carrera, string nombre, string direccion, string telefono) {
	pnodoB nuevo = new nodoB(codigo, carrera, nombre, direccion, telefono);
	insertar(nuevo);
}

void ProfesoresEstudiantes::crearNodoProf(int codigo, int carrera, int categoria, string nombre) {
	pnodoB nuevo = new nodoB(codigo, carrera, categoria, nombre);
	insertar(nuevo);
}

bool ProfesoresEstudiantes::buscarNodo(pPagina actual, int cl, int &k) {
	int index;
	bool encontrado;
	if (cl < actual->getClave(1)->codigo) {
		encontrado = false;
		index = 0;
	}else {
		index = actual->getCuenta();
		while (cl < actual->getClave(index)->codigo && (index > 1))
			index--;
		encontrado = cl == actual->getClave(index)->codigo;
	}
	k = index;
	return encontrado;
}

bool ProfesoresEstudiantes::buscar(int cl) {
	int k = 0;
	return buscar(raiz, cl, k);
}

bool ProfesoresEstudiantes::buscar(pPagina actual, int cl, int &k) { //empieza con raiz, codigo y 0
	if (actual == NULL) {
		return false;
	}
	else {
		bool esta = buscarNodo(actual, cl, k);
		if (esta)
			return true;
		else
			return buscar(actual->getRama(k), cl, k);
	}
}

pnodoB ProfesoresEstudiantes::buscarN(pPagina actual, int cl, int &k) { //empieza con raiz, codigo y 0
	if (actual == NULL) {
		return false;
	}
	else {
		bool esta = buscarNodo(actual, cl, k);
		if (esta)
			return actual->getClave(k);
		else
			return buscarN(actual->getRama(k), cl, k);
	}
}

int ProfesoresEstudiantes::buscarCarrera(int cl) {
	int k = 0;
	return buscarCarrera(raiz, cl, k);
}

int ProfesoresEstudiantes::buscarCarrera(pPagina actual, int cl, int &k) { //empieza con raiz, codigo y 0
	if (actual == NULL) {
		return 0;
	}
	else {
		bool esta = buscarNodo(actual, cl, k);
		if (esta)
			return actual->getClave(k)->carrera;
		else
			return buscarCarrera(actual->getRama(k), cl, k);
	}
}

void ProfesoresEstudiantes::insertar(pnodoB cl){
	insertar(raiz, cl);
}

void ProfesoresEstudiantes::insertar(pPagina r, pnodoB cl) {
	bool subeArriba;
	pnodoB mediana;
	pPagina xder;
	subeArriba = empujar(r, cl, mediana, xder);
	if (subeArriba) {
		pPagina p;
		p = new Pagina(orden);
		p->setCuenta(1);
		p->setClave(1, mediana);
		p->setRama(0, r); // claves menores
		p->setRama(1, xder); // claves mayores
		r = p;
		raiz = r;
	}
}

bool ProfesoresEstudiantes::empujar(pPagina actual, pnodoB cl, pnodoB &mediana, pPagina &xder) {
	int k;
	bool subeArriba = false;
	if (actual == NULL){
		subeArriba = true;
		mediana = cl;
		xder = NULL;
	}else {
		bool esta;
		esta = buscarNodo(actual, cl->codigo, k);
		if (esta)
			throw "\nClave duplicada";
		subeArriba = empujar(actual->getRama(k), cl, mediana, xder);
		if (subeArriba) {
			if (actual->nodoLLeno()) {
				subeArriba = true;
				dividirNodo(actual, mediana, xder, k);
			}else {
				subeArriba = false;
				meterPagina(actual, mediana, xder, k);
			}
		}
	}
	return subeArriba;
}

void ProfesoresEstudiantes::meterPagina(pPagina actual, pnodoB cl, pPagina ramaDr, int k) {
	for (int i = actual->getCuenta(); i >= k + 1; i--){
		actual->setClave(i + 1, actual->getClave(i));
		actual->setRama(i + 1, actual->getRama(i));
	}
	actual->setClave(k + 1, cl);
	actual->setRama(k + 1, ramaDr);
	actual->setCuenta(actual->getCuenta() + 1);
}

void ProfesoresEstudiantes::dividirNodo(pPagina actual, pnodoB &mediana, pPagina &xder, int pos) {
	int i, posMdna, k;
	pPagina nuevaPag;
	k = pos;
	if (k <= orden / 2) {
		posMdna = orden / 2;
	}
	else {
		posMdna = (orden / 2) + 1;
	}
	nuevaPag = new Pagina(orden);
	for (i = posMdna + 1; i < orden; i++)
	{
		nuevaPag->setClave(i - posMdna, actual->getClave(i));
		nuevaPag->setRama(i - posMdna, actual->getRama(i));
	}
	nuevaPag->setCuenta((orden - 1) - posMdna);
	actual->setCuenta(posMdna);
	if (k <= orden / 2)
		meterPagina(actual, mediana, xder, pos);
	else
	{
		pos = k - posMdna;
		meterPagina(nuevaPag, mediana, xder, pos);
	}
	mediana = actual->getClave(actual->getCuenta());
	nuevaPag->setRama(0, actual->getRama(actual->getCuenta()));
	actual->setCuenta(actual->getCuenta() - 1);
	xder = nuevaPag;
}

void ProfesoresEstudiantes::leerProfesores(string txt) {
	ifstream archivo;
	string texto;
	archivo.open(txt, ios::in);
	if (archivo.fail()) {
		cout << "No se pudo abrir el archivo\n";
		system("pause");
		exit(1);
	}
	while (!archivo.eof()) {
		getline(archivo, texto);
		string datos[4];
		int actual = 0;
		for (int i = 0; i != texto.length(); i++) {
			if (texto[i] == ';') {
				actual++;
				i++;
			}
			datos[actual] += texto[i];
		}
		crearNodoProf(stoi(datos[0]), stoi(datos[1]), stoi(datos[2]), datos[3]);
	}
}

void ProfesoresEstudiantes::leerEstudiantes(string txt) {
	ifstream archivo;
	string texto;
	archivo.open(txt, ios::in);
	if (archivo.fail()) {
		cout << "No se pudo abrir el archivo\n";
		system("pause");
		exit(1);
	}
	while (!archivo.eof()) {
		getline(archivo, texto);
		string datos[5];
		int actual = 0;
		for (int i = 0; i != texto.length(); i++) {
			if (texto[i] == ';') {
				actual++;
				i++;
			}
			datos[actual] += texto[i];
		}
		crearNodoEst(stoi(datos[0]), stoi(datos[1]), datos[2], datos[3], datos[4]);
	}
}

//imprime el arbol
void ProfesoresEstudiantes::escribir() {
	escribir(raiz);
}

void ProfesoresEstudiantes::escribir(pPagina r) {
	int i;
	for (i = 1; i < r->getCuenta() + 1; ++i)
		cout << r->getClave(i)->codigo << "-";
	cout << "\n-----------------" << endl;
	for (i = 0; i <= r->getCuenta(); ++i) {
		if (r->getRama(i) != NULL)
			escribir(r->getRama(i));
	}
	return;
}

class CursoEstudiante {
public:
	CursoEstudiante(int pCodigo) {
		codigo = pCodigo;
		estado = 1;
		Hizq = NULL;
		Hder = NULL;
		padre = NULL;
	};
	void InsertarNivel(int n) { nivel = n; };
private:
	CursoEstudiante * Hizq, *Hder, *padre, *raiz, *anterior;
	int cuenta = 0;
	int nivel = 0;
	int codigo;
	int estado;

	friend class Grupo;
	friend class ListaSimple;
	friend class Cola;
};
typedef CursoEstudiante *pCursoEstudiante;

class Grupo {
public:
	Grupo(string datos[], Grupo *pPadre) {
		carrera = stoi(datos[0]);
		curso = stoi(datos[1]);
		codigo = stoi(datos[2]);
		profesor = stoi(datos[3]);
		cupos = stoi(datos[4]);
		matriculados = stoi(datos[5]);
		congelados = stoi(datos[6]);
		desmatriculados = 0;
		color = "R";
		padre = pPadre;
	}
	void cambiarColor();
	void crearNodo(int valor);
	void reparto(pCursoEstudiante);
	void insertarNodo(pCursoEstudiante, pCursoEstudiante);
	void giro(pCursoEstudiante, pCursoEstudiante, pCursoEstudiante);
	bool buscarNodo(pCursoEstudiante raiz, int codigo);
private:
	int carrera;
	int curso;
	int codigo;
	int profesor;
	string color;
	int cupos;
	int matriculados;
	int congelados;
	int desmatriculados;
	mutex atencion;
	//pCursoEstudiante primerEstudiante;
	Grupo *Hizq, *Hder, *padre;
	pCursoEstudiante raizCursoEstudiante;

	friend class Curso;
	friend class Carrera;
};
typedef Grupo *pGrupo;

void Grupo::cambiarColor() {
	if (color == "R") {
		color = "N";
	}else {
		color = "R";
	}
}

void Grupo::crearNodo(int valor) {
	pCursoEstudiante temp = new CursoEstudiante(valor);
	insertarNodo(raizCursoEstudiante, temp);
}

bool Grupo::buscarNodo(pCursoEstudiante r, int codigo) {
	if (r == NULL) {
		return false;
	}
	if (codigo < r->codigo) {
		buscarNodo(r->Hizq, codigo);
	}
	else if (codigo == r->codigo) {
		return true;
	}
	else {
		buscarNodo(r->Hder, codigo);
	}
}

void Grupo::reparto(pCursoEstudiante temp) {
	pCursoEstudiante segundo = temp->Hder;
	pCursoEstudiante tercero = NULL;
	if (segundo != NULL) {
		tercero = temp->Hder->Hder;
	}
	if (segundo != NULL && tercero != NULL) {
		if (temp->nivel == segundo->nivel && temp->nivel == tercero->nivel) {
			if (temp->padre != NULL) {
				if (temp->padre->Hizq == temp) {
					temp->padre->Hizq = segundo;
					giro(segundo, temp->padre, tercero);
					if (temp->padre->anterior != NULL) {
						temp->padre->anterior->Hder = segundo;
						segundo->anterior = temp->padre->anterior;
					}
					temp->padre->anterior = segundo;
					temp->padre->padre = NULL;
				}
				else {
					temp->padre->Hder = segundo;
					segundo->anterior = temp->padre;
				}
			}
			if (tercero->padre == NULL) {
				tercero->padre = segundo;
			}
			temp->padre = segundo;
			temp->Hder = segundo->Hizq;
			if (temp->Hder != NULL)
				temp->Hder->padre = temp;
			segundo->Hizq = temp;
			segundo->nivel = temp->nivel + 1;
			tercero->anterior = NULL;
			if (segundo->anterior == temp)
				segundo->anterior = NULL;
			while (segundo->anterior != NULL) {
				segundo = segundo->anterior;
			}
			if (raizCursoEstudiante == temp) {
				raizCursoEstudiante = segundo;
			}
			reparto(segundo);
		}
	}
}

void Grupo::insertarNodo(pCursoEstudiante temp, pCursoEstudiante ins) {
	if (raizCursoEstudiante == NULL){
		ins->nivel = 1;
		raizCursoEstudiante = ins;
	}else if (ins->codigo < temp->codigo){
		if (temp->Hizq != NULL)
			return insertarNodo(temp->Hizq, ins);
		if (temp->anterior != NULL) {
			ins->Hder = temp;
			temp->anterior->Hder = ins;
			ins->anterior = temp->anterior;
		}
		temp->Hizq = ins;
		temp->anterior = ins;
		ins->padre = NULL;
		giro(ins, temp, NULL);
		ins->nivel = temp->nivel;
		while (ins->anterior != NULL) {
			ins = ins->anterior;
		}
		reparto(ins);
	}else if (ins->codigo > temp->codigo){
		if (temp->Hder != NULL)
			return insertarNodo(temp->Hder, ins);
		temp->Hder = ins;
		ins->anterior = temp;
		ins->padre = NULL;
		ins->nivel = temp->nivel;
		while (ins->anterior != NULL) {
			ins = ins->anterior;
		}
		reparto(ins);
	}
}

void Grupo::giro(pCursoEstudiante ins, pCursoEstudiante temp, pCursoEstudiante rama) {
	if (temp == raizCursoEstudiante) {
		raizCursoEstudiante = ins;
	}
	temp->Hizq = rama;
	ins->Hder = temp;
	if (temp->padre != NULL) {
		if (temp->padre->Hizq == temp) {
			temp->padre->Hizq = ins;
			ins->padre = temp->padre;
			temp->padre = NULL;
		}
		else {
			temp->padre->Hder = ins;
			ins->padre = temp->padre;
			temp->padre = NULL;
		}
	}
	if (rama != NULL) {
		rama->padre = temp;
	}
}

class Curso {
public:
	Curso(int pCodigo, string pNombre,Curso *pPadre) {
		codigo = pCodigo;
		nombre = pNombre;
		FB = 0;
		padre = pPadre;
		raiz = NULL;
	}
	pGrupo raiz;
	bool Hh;
	void InsertarGrupo(pGrupo padre, pGrupo ra, string datos[], string lado);
	void reglaDelTio(pGrupo abuelo);
	void RotacionSimpleDerecha(pGrupo abuelo, pGrupo padre, pGrupo hijo);
	void RotacionSimpleIzquierda(pGrupo abuelo, pGrupo padre, pGrupo hijo);
	void RotacionDobleIzquierda(pGrupo abuelo, pGrupo padre, pGrupo hijo);
	void RotacionDobleDerecha(pGrupo abuelo, pGrupo padre, pGrupo hijo);
	void insertarEstudiante(string datos[]);
	void InordenR(pGrupo R, string lado);
private:
	int codigo;
	string nombre;
	int FB;
	Curso *padre, *Hizq, *Hder;

	friend class Carrera;
};
typedef Curso *pCurso;

void Curso::InsertarGrupo(pGrupo padre, pGrupo ra, string datos[], string lado) {
	Hh = false;
	if (raiz == NULL) {
		raiz = new Grupo(datos, padre);
		raiz->cambiarColor();
	}
	else if (ra == NULL) {
		ra = new Grupo(datos, padre);
		if (lado == "izq") {
			padre->Hizq = ra;
		}
		else {
			padre->Hder = ra;
		}
		Hh = true;
	}
	else {
		if (stoi(datos[2]) < ra->codigo) {
			InsertarGrupo(ra, ra->Hizq, datos, "izq");
			if (Hh) {
				if (ra->Hizq->color == "R" && ra->color == "R") {
					pGrupo abuelo = ra->padre;
					string colorTio = "";
					if (lado == "izq") {
						if (abuelo != NULL && abuelo->Hder != NULL) {
							colorTio = abuelo->Hder->color;
						}
					}
					else {
						if (abuelo != NULL && abuelo->Hizq != NULL) {
							colorTio = abuelo->Hizq->color;
						}
					}
					if (colorTio == "R") {
						reglaDelTio(abuelo);
					}
					else {
						if (lado == "izq") {
							RotacionSimpleIzquierda(padre->padre, padre, ra);
						}
						else {
							RotacionDobleDerecha(padre->padre, padre, ra);
						}
						Hh = false;
					}
				}
			}
		}else if(stoi(datos[2])>ra->codigo){
			InsertarGrupo(ra, ra->Hder, datos, "der");
			if (Hh) {
				if (ra->Hder->color == "R" && ra->color == "R") {
					pGrupo abuelo = ra->padre;
					string colorTio = "";
					if (lado == "izq") {
						if (abuelo != NULL && abuelo->Hder != NULL) {
							colorTio = abuelo->Hder->color;
						}
					}
					else {
						if (abuelo != NULL && abuelo->Hizq != NULL) {
							colorTio = abuelo->Hizq->color;
						}
					}
					if (colorTio == "R") {
						reglaDelTio(abuelo);
					}
					else {
						if (lado == "der") {
							RotacionSimpleDerecha(padre->padre, padre, ra);
						}
						else {
							RotacionDobleIzquierda(padre->padre, padre, ra);
						}
						Hh = false;
					}
				}
			}
		}else
			return;
	}
}

void Curso::reglaDelTio(pGrupo abuelo) {
	pGrupo izq = abuelo->Hder;
	pGrupo der = abuelo->Hizq;
	izq->cambiarColor();
	der->cambiarColor();
	if (abuelo != raiz) {
		abuelo->cambiarColor();
	}
}

void Curso::RotacionSimpleDerecha(pGrupo abuelo, pGrupo padre, pGrupo hijo) {
	padre->cambiarColor();
	hijo->cambiarColor();
	padre->Hder = hijo->Hizq;
	if (hijo->Hizq != NULL) {
		hijo->Hizq->padre = padre;
	}
	hijo->Hizq = padre;
	padre->padre = hijo;
	if (abuelo == NULL) {
		raiz = hijo;
	}
	else if (abuelo->Hder == padre) {
		abuelo->Hder = hijo;
	}
	else {
		abuelo->Hizq = hijo;
	}
	hijo->padre = abuelo;
}

void Curso::RotacionSimpleIzquierda(pGrupo abuelo, pGrupo padre, pGrupo hijo) {
	padre->cambiarColor();
	hijo->cambiarColor();
	padre->Hizq = hijo->Hder;
	if (hijo->Hder != NULL) {
		hijo->Hder->padre = padre;
	}
	hijo->Hder = padre;
	padre->padre = hijo;
	if (abuelo == NULL) {
		raiz = hijo;
	}
	else if (abuelo->Hder == padre) {
		abuelo->Hder = hijo;
	}
	else {
		abuelo->Hizq = hijo;
	}
	hijo->padre = abuelo;
}

void Curso::RotacionDobleIzquierda(pGrupo abuelo, pGrupo padre, pGrupo hijo) {//padre n, hijo n1 , nieto n2
	pGrupo nieto = hijo->Hder;
	padre->cambiarColor();
	nieto->cambiarColor();
	padre->Hizq = nieto->Hder;
	if (nieto->Hder != NULL) {
		nieto->Hder->padre = padre;
	}
	nieto->Hder = padre;
	padre->padre = nieto;
	hijo->Hder = nieto->Hizq;
	if (nieto->Hizq != NULL) {
		nieto->Hizq->padre = hijo;
	}
	nieto->Hizq = hijo;
	hijo->padre = nieto;
	if (abuelo == NULL) {
		raiz = nieto;
	}
	else if (abuelo->Hder == padre) {
		abuelo->Hder = nieto;
	}
	else {
		abuelo->Hizq = nieto;
	}
	nieto->padre = abuelo;
}

void Curso::RotacionDobleDerecha(pGrupo abuelo, pGrupo padre, pGrupo hijo) { // padre n, hijo n1
	pGrupo nieto = hijo->Hizq;
	padre->cambiarColor();
	nieto->cambiarColor();
	padre->Hder = nieto->Hizq;
	if (nieto->Hizq != NULL) {
		nieto->Hizq->padre = padre;
	}
	nieto->Hizq = padre;
	padre->padre = nieto;
	hijo->Hizq = nieto->Hder;
	if (nieto->Hder != NULL) {
		nieto->Hder->padre = hijo;
	}
	nieto->Hder = hijo;
	hijo->padre = nieto;
	if (abuelo == NULL) {
		raiz = nieto;
	}
	else if (abuelo->Hder == padre) {
		abuelo->Hder = nieto;
	}
	else {
		abuelo->Hizq = nieto;
	}
	nieto->padre = abuelo;
}

void Curso::insertarEstudiante(string datos[]) {
	pGrupo aux = raiz;
	int codGrupo = stoi(datos[2]);
	while (aux != NULL){
		if (codGrupo == aux->codigo) {
			aux->crearNodo(stoi(datos[0]));
			break;
		}else if(codGrupo < aux->codigo){
			aux = aux->Hizq;
		}else {
			aux = aux->Hder;
		}
	}
}

void Curso::InordenR(pGrupo R, string lado) {
	if (R != NULL) {
		InordenR(R->Hizq, "izq");
		cout << R->codigo << R->color;
		InordenR(R->Hder, "der");
	}
}

class Carrera {
public:
	Carrera(int pCodigo, string pNombre) {
		codigo = pCodigo;
		nombre = pNombre;
	}
	void InsertarBalanceado(pCurso padre, pCurso ra, int x, string nombre, string lado);
	void RotacionSimpleDerecha(pCurso abuelo, pCurso padre, pCurso hijo);
	void RotacionSimpleIzquierda(pCurso abuelo, pCurso padre, pCurso hijo);
	void RotacionDobleIzquierda(pCurso abuelo, pCurso padre, pCurso hijo);
	void RotacionDobleDerecha(pCurso abuelo, pCurso padre, pCurso hijo);
	void insertarGrupo(string datos[]);
	void insertarEstudiante(string datos[]);
	void InordenR(pCurso R);
private:
	bool Hh;
	int codigo;
	string nombre;
	Carrera *Hizq, *Hder;
	pCurso primerCurso;

	friend class ArbolCarreras;
};
typedef Carrera *pCarrera;

void Carrera::InsertarBalanceado(pCurso padre, pCurso ra, int x, string nombre, string lado) {
	Hh = false;
	if (primerCurso == NULL) {
		primerCurso = new Curso(x, nombre, padre);
	}
	else if (ra == NULL) {
		ra = new Curso(x, nombre, padre);
		if (lado == "izq") {
			padre->Hizq = ra;
		}
		else {
			padre->Hder = ra;
		}
		Hh = true;
	}
	else {
		if (x <= ra->codigo) {
			InsertarBalanceado(ra, ra->Hizq, x, nombre, "izq");
			if (Hh) {
				switch (ra->FB) {
				case 1: ra->FB = 0;
					Hh = false;
					break;
				case 0: ra->FB = -1;
					break;
				case -1:
					pCurso hijo = ra->Hizq;
					if (hijo->FB == -1) {
						RotacionSimpleIzquierda(ra->padre, ra, hijo);
					}
					else {
						RotacionDobleIzquierda(ra->padre, ra, hijo);
					}
					Hh = false;
					break;
				}
			}
		}else if(x > ra->codigo){
			InsertarBalanceado(ra, ra->Hder, x, nombre, "der");
			if (Hh) {
				switch (ra->FB) {
				case -1: ra->FB = 0;
					Hh = false;
					break;
				case 0: ra->FB = 1;
					break;
				case 1:
					pCurso hijo = ra->Hder;
					if (hijo->FB == 1) {
						RotacionSimpleDerecha(ra->padre, ra, hijo);
					}
					else {
						RotacionDobleDerecha(ra->padre, ra, hijo);
					}
					Hh = false;
					break;
				}
			}
		}else {
			return;
		}
	}
}

void Carrera::RotacionSimpleDerecha(pCurso abuelo, pCurso padre, pCurso hijo) {
	padre->Hder = hijo->Hizq;
	if (hijo->Hizq != NULL) {
		hijo->Hizq->padre = padre;
	}
	hijo->Hizq = padre;
	padre->padre = hijo;
	if (hijo->FB == 1) {
		padre->FB = 0;
		hijo->FB = 0;
	}
	else {
		padre->FB = 1;
		hijo->FB = -1;
	}
	if (abuelo == NULL) {
		primerCurso = hijo;
	}
	else if (abuelo->Hder == padre) {
		abuelo->Hder = hijo;
	}
	else {
		abuelo->Hizq = hijo;
	}
	hijo->padre = abuelo;
}

void Carrera::RotacionSimpleIzquierda(pCurso abuelo, pCurso padre, pCurso hijo) {
	padre->Hizq = hijo->Hder;
	if (hijo->Hder != NULL) {
		hijo->Hder->padre = padre;
	}
	hijo->Hder = padre;
	padre->padre = hijo;
	if (hijo->FB == -1) {
		padre->FB = 0;
		hijo->FB = 0;
	}
	else {
		padre->FB = -1;
		hijo->FB = -1;
	}
	if (abuelo == NULL) {
		primerCurso = hijo;
	}
	else if (abuelo->Hder == padre) {
		abuelo->Hder = hijo;
	}
	else {
		abuelo->Hizq = hijo;
	}
	hijo->padre = abuelo;
}

void Carrera::RotacionDobleIzquierda(pCurso abuelo, pCurso padre, pCurso hijo) {
	pCurso nieto = hijo->Hder;
	padre->Hizq = nieto->Hder;
	if (nieto->Hder != NULL) {
		nieto->Hder->padre = padre;
	}
	nieto->Hder = padre;
	padre->padre = nieto;
	hijo->Hder = nieto->Hizq;
	if (nieto->Hizq != NULL) {
		nieto->Hizq->padre = hijo;
	}
	nieto->Hizq = hijo;
	hijo->padre = nieto;
	if (nieto->FB == 1) {
		hijo->FB = -1;
	}
	else {
		hijo->FB = 0;
	}
	if (nieto->FB == -1) {
		padre->FB = 1;
	}
	else {
		padre->FB = 0;
	}
	nieto->FB = 0;
	if (abuelo == NULL) {
		primerCurso = nieto;
	}
	else if (abuelo->Hder == padre) {
		abuelo->Hder = nieto;
	}
	else {
		abuelo->Hizq = nieto;
	}
	nieto->padre = abuelo;
}

void Carrera::RotacionDobleDerecha(pCurso abuelo, pCurso padre, pCurso hijo) {
	pCurso nieto = hijo->Hizq;
	padre->Hder = nieto->Hizq;
	if (nieto->Hizq != NULL) {
		nieto->Hizq->padre = padre;
	}
	nieto->Hizq = padre;
	padre->padre = nieto;
	hijo->Hizq = nieto->Hder;
	if (nieto->Hder != NULL) {
		nieto->Hder->padre = hijo;
	}
	nieto->Hder = hijo;
	hijo->padre = nieto;
	if (nieto->FB == 1) {
		padre->FB = -1;
	}
	else {
		padre->FB = 0;
	}
	if (nieto->FB == -1) {
		hijo->FB = 1;
	}
	else {
		hijo->FB = 0;
	}
	nieto->FB = 0;
	if (abuelo == NULL) {
		primerCurso = nieto;
	}
	else if (abuelo->Hder == padre) {
		abuelo->Hder = nieto;
	}
	else {
		abuelo->Hizq = nieto;
	}
	nieto->padre = abuelo;
}

void Carrera::insertarGrupo(string datos[]) {
	pCurso aux = primerCurso;
	int codigoCurso = stoi(datos[1]);
	while (aux != NULL) {
		if (codigoCurso == aux->codigo) {
			aux->InsertarGrupo(NULL, aux->raiz, datos, "");
			break;
		}else if (codigoCurso < aux->codigo) {
			aux = aux->Hizq;
		}else {
			aux = aux->Hder;
		}
	}
}

void Carrera::insertarEstudiante(string datos[]) {
	pCurso aux = primerCurso;
	int codCurso = stoi(datos[1]);
	while (aux != NULL) {
		if (codCurso == aux->codigo) {
			aux->insertarEstudiante(datos);
			break;
		}else if (codCurso < aux->codigo) {
			aux = aux->Hizq;
		}else {
			aux = aux->Hder;
		}
	}
}

void Carrera::InordenR(pCurso R) {
	return R->InordenR(R->raiz, "");
}

class ArbolCarreras {
public:
	pCarrera raiz;
	ArbolCarreras() { raiz = NULL; };
	void Insertar(pCarrera padre, pCarrera ra, string lado, int x, string nombre);
	void leerCarrera(string txt);
	void cargarCurso(string txt);
	void cargarGrupos(string txt, pProfesoresEstudiantes profesores);
	void cargarEstudiantesCurso(string txt, pProfesoresEstudiantes lista);
	void InordenR(pCarrera R);
};
typedef ArbolCarreras *pArbolCarreras;

void ArbolCarreras::Insertar(pCarrera padre, pCarrera ra, string lado, int x, string nombre) {
	if (raiz == NULL) {
		raiz = new Carrera(x, nombre);
	}else if (ra == NULL) {
		if (lado == "izq") {
			padre->Hizq = new Carrera(x, nombre);
		}else {
			padre->Hder = new Carrera(x, nombre);
		}
	}else {
		if (x<ra->codigo) {
			Insertar(ra, ra->Hizq, "izq", x, nombre);
		}else {
			if (x>ra->codigo) {
				Insertar(ra, ra->Hder, "der", x, nombre);
			}
		}
	}
}

void ArbolCarreras::leerCarrera(string txt) {
	ifstream archivo;
	string texto;
	archivo.open(txt, ios::in);
	if (archivo.fail()) {
		cout << "No se pudo abrir el archivo\n";
		system("pause");
		exit(1);
	}
	while (!archivo.eof()) {
		getline(archivo, texto);
		string datos[2];
		int actual = 0;
		for (int i = 0; i != texto.length(); i++) {
			if (texto[i] == ';') {
				actual++;
				i++;
			}
			datos[actual] += texto[i];
		}
		Insertar(NULL, raiz, "", stoi(datos[0]), datos[1]);
	}
}

void ArbolCarreras::cargarCurso(string txt) {
	ifstream archivo;
	string texto;
	archivo.open(txt, ios::in);
	if (archivo.fail()) {
		cout << "No se pudo abrir el archivo\n";
		system("pause");
		exit(1);
	}
	while (!archivo.eof()) {
		getline(archivo, texto);
		string datos[3];
		int actual = 0;
		for (int i = 0; i != texto.length(); i++) {
			if (texto[i] == ';') {
				actual++;
				i++;
			}
			datos[actual] += texto[i];
		}
		int codCarrera = stoi(datos[0]);
		pCarrera aux = raiz;
		while (aux != NULL) {
			if (aux->codigo == codCarrera) {
				aux->InsertarBalanceado(NULL, aux->primerCurso, stoi(datos[1]), datos[2], "");
				break;
			}else if(codCarrera < aux->codigo){
				aux = aux->Hizq;
			}else {
				aux = aux->Hder;
			}
		}
	}
}

void ArbolCarreras::cargarGrupos(string txt, pProfesoresEstudiantes profesores) {//arbolProfesores
	ifstream archivo;
	string texto;
	archivo.open(txt, ios::in);
	if (archivo.fail()) {
		cout << "No se pudo abrir el archivo\n";
		system("pause");
		exit(1);
	}
	while (!archivo.eof()) {
		getline(archivo, texto);
		string datos[7];
		int actual = 0;
		for (int i = 0; i != texto.length(); i++) {
			if (texto[i] == ';') {
				actual++;
				i++;
			}
			datos[actual] += texto[i];
		}
		if (profesores->buscar(stoi(datos[3]))){//arbolProfesores.buscarProfesor(datos[3])
			pCarrera aux = raiz;
			int codCarrera = stoi(datos[0]);
			while (aux != NULL) {
				if (codCarrera == aux->codigo) {
					aux->insertarGrupo(datos);
					break;
				}else if (codCarrera < aux->codigo) {
					aux = aux->Hizq;
				}else{
					aux = aux->Hder;
				}
			}
		}
	}
	archivo.close();
}

void ArbolCarreras::cargarEstudiantesCurso(string txt, pProfesoresEstudiantes estudiantes) {
	ifstream archivo;
	string texto;
	archivo.open(txt, ios::in);
	if (archivo.fail()) {
		cout << "No se pudo abrir el archivo\n";
		system("pause");
		exit(1);
	}
	while (!archivo.eof()) {
		getline(archivo, texto);
		string datos[3];
		int actual = 0;
		for (int i = 0; i != texto.length(); i++) {
			if (texto[i] == ';') {
				actual++;
				i++;
			}
			datos[actual] += texto[i];
		}
		int codCarrera = estudiantes->buscarCarrera(stoi(datos[0]));
		if (codCarrera != 0) {
			pCarrera aux = raiz;
			while (aux != NULL) {
				if (codCarrera == aux->codigo) {
					aux->insertarEstudiante(datos);
					break;
				}else if (codCarrera < aux->codigo) {
					aux = aux->Hizq;
				}else {
					aux = aux->Hder;
				}
			}
		}

	}
	archivo.close();
}


void ArbolCarreras::InordenR(pCarrera R){
	return R->InordenR(R->primerCurso);
}

int main() {
	ProfesoresEstudiantes estudiantes;
	estudiantes.leerEstudiantes("Estudiantes.txt");
	ProfesoresEstudiantes profesores;
	profesores.leerProfesores("Profesores.txt");
	ArbolCarreras carreras;
	carreras.leerCarrera("Carreras.txt");
	carreras.cargarCurso("Cursos.txt");
	carreras.cargarGrupos("Grupos.txt", &profesores);//con profesores
	carreras.cargarEstudiantesCurso("Estudiante-Curso.txt", &estudiantes);
	/*ListaString lista;
	/*lista.crearListaAtencion("Atencion.txt");
	/*ListaMostradores mostradores;
	/*Cola cola(&lista, &mostradores, &estudiantes, &profesores, &carreras);
	/*cola.threadAtender();
	/*cola.crearReportes();*/
	system("pause");
	return 0;
};