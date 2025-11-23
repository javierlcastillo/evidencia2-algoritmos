// Actividad intetradora 2
// Autor: Javier Luis Castillo Solórzano
// Matrícula: A01658415

#include <iostream>
#include <vector>
#include <unordered_map>

using namespace std;

struct Colonia{
    int id;
    string nombre;
    int x, y;
    bool esCentral;
};

struct Graph {
    int n, m;
    unordered_map<string, Colonia> colonias;
    unordered_map<string, int> indiceColonias;  
    vector<pair<int, pair<int, int>>> edges;
    vector<pair<int,int>> selectedEdges;
    Graph(int _n, int _m){
        this-> n = _n;
        this-> m  = _m;
    }

    // u = salida del arco
	// v = llegada del arco
	// w = costo del arco

	void addEdge(string u, string v, int w){ // -> u vetice1, v vertice2, w costo
        auto it_u = indiceColonias.find(u);
        auto it_v = indiceColonias.find(v)
        if (it_u == indiceColonias.end() || it_v == indiceColonias.end()){
            cout << "Colonia a conectar no encontrada" << endl; 
            return;
        } else{
		    edges.push_back({w, {it_u->second, it_v->second}}); // -> utilizado en kruskal: first = costo, second = conexión
        }
    }

    /* --- Manejo de colonias en <unoredered_map> ---
     * ----------------------------------------------
     * Se verifica que la colonia no exista para después asignarle un id numerico
     * para que sea utilizado como indice en kruskal. 
     * 
    */
    void addColoniaInMap(string _nombre, int _x, int _y, int _esCentral){
        auto it = colonias.find(_nombre);
        if (it == colonias.end()){
            Colonia temp;
            indiceColonias[_nombre] = colonias.size();
            temp.x = _x;
            temp.y = _y;
            temp.esCentral = _esCentral;
            temp.nombre = _nombre; 
            colonias[_nombre] = temp;
        }
    }
    
};

int main (){
    int n, m, k, q;
    cin >> n >> m >> k >> q;
    Graph g(n, m);
    int x, y, central; 
    string nombre1, nombre2;
    for(int i = 0; i < n; i++){
        cin >> nombre1 >> x >> y >> central;
        g.addColoniaInMap(nombre1, x, y, central);
    }
    for(int i = 0; i < m; i++){

        g.addEdge()
    }

    return 0;
}