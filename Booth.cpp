#include <iostream>
#include <bitset>
using namespace std;

//////////////////////////////////////////////////////////////////////
//////////////////////////// Suma binaria ////////////////////////////
//////////////////////////////////////////////////////////////////////

bitset<16> sumaBinaria(bitset<16> A, bitset<16> B, int n) {
    
    bitset<16> resultado;
    bool acarreo = 0;  // Inicializar acarreo como 0
    
    for (int i = 0; i < n; i++) {
        // Realizar la suma bit a bit y se agrega el acarreo anterior
        resultado[i] = A[i] ^ B[i] ^ acarreo;
        
        // Acarreo para la siguiente posición
        acarreo = (A[i] & B[i]) | (acarreo & (A[i] ^ B[i]));
    }
    
    return resultado;

}

//////////////////////////////////////////////////////////////////////
//////////////////// Conversión a complemento a 2 ////////////////////
//////////////////////////////////////////////////////////////////////

bitset<16> complementoADos(bitset<16> bits, int n) {

    bitset<16> invertido;
    
    // Invertimos la cadena de bits (negación de cada bit)
    for (int i = 0; i < n; i++) {
        invertido[i] = !bits[i];
    }
    
    // Sumar uno al resultado
    bitset<16> uno(1);
    return sumaBinaria(invertido, uno, n);

}

//////////////////////////////////////////////////////////////////////
//////////////////////// Conversión a binario ////////////////////////
//////////////////////////////////////////////////////////////////////

bitset<16> convertirABinario(int numero, int n) {

    bitset<16> resultado;
    
    // Números positivos
    if (numero >= 0) {
        resultado = bitset<16>(numero);

    // Números negativos
    } else {

        unsigned int valorAbs = abs(numero);
        resultado = bitset<16>(valorAbs);
        
        // Transformar el número a complemento-2
        resultado = complementoADos(resultado, n);
    }
    
    return resultado;
}

//////////////////////////////////////////////////////////////////////
//////////////////// Desplazamiento a la derecha /////////////////////
//////////////////////////////////////////////////////////////////////

void desplazamientoDerecha(bitset<16>& A, bitset<16>& Q, bool& Q_1, int n) {

    bool bitMasSignificativo = A[n - 1]; // Mantener el bit de signo antes del cambio
    
    // Guardar el bit menos significativo antes del desplazamiento
    Q_1 = Q[0];
    
    // Desplazamos Q a la derecha
    for (int i = 0; i < n - 1; i++) {
        Q[i] = Q[i + 1];
    }
    
    // Reemplazar el bit menos significativo de A se mueve por el bit más significativo de Q
    Q[n - 1] = A[0];
    
    // Desplazamos A a la derecha
    for (int i = 0; i < n - 1; i++) {
        A[i] = A[i + 1];
    }
    
    // Mantenemos el bit de signo de A
    A[n - 1] = bitMasSignificativo;
}

//////////////////////////////////////////////////////////////////////
//////////////////////// Algoritmo de Booth //////////////////////////
//////////////////////////////////////////////////////////////////////

void algoritmoBooth(int multiplicando, int multiplicador, int n) {

    bitset<16> M = convertirABinario(multiplicando, n);
    bitset<16> Q = convertirABinario(multiplicador, n);
    bitset<16> A = 0;
    bool Q_1 = 0;
    int contador = n; // Número de iteraciones

    cout << " " << endl;
    cout << "Valores iniciales:" << endl;
    cout << "M (multiplicando): " << M << endl;
    cout << "Q (multiplicador): " << Q << endl;
    cout << "A: " << A << endl;
    cout << "Q_1: " << Q_1 << endl << endl;
    cout << "=================================================================" << endl;
    cout << " " << endl;

    while (contador > 0) {

        cout << "=================================================================" << endl;
        cout << " " << endl;
        cout << "Iteración " << n - contador + 1 << ":" << endl;
        cout << "A: " << A << ", Q: " << Q << ", Q_1: " << Q_1 << endl;
        
        //////////////////////////////////////////////////////////////////////
        /////////////////////// Comprobar el estado //////////////////////////
        //////////////////////////////////////////////////////////////////////

        // Comparamos los dos bits menos significativos (Q0 y Q_1)
        if (Q[0] == 1 && Q_1 == 0) {
            // Si Q0=1 y Q_1=0, restamos: A = A - M
            cout << "Q0=1, Q_1=0 => A = A - M" << endl;
            A = sumaBinaria(A, complementoADos(M, n), n);
        } else if (Q[0] == 0 && Q_1 == 1) {
            // Si Q0=0 y Q_1=1, sumamos: A = A + M
            cout << "Q0=0, Q_1=1 => A = A + M" << endl;
            A = sumaBinaria(A, M, n);
        } else {
            cout << "Q0=Q_1 => Sin operación" << endl;
        }

        // Desplazamiento a la derecha
        cout << "Desplazamiento" << endl;
        desplazamientoDerecha(A, Q, Q_1, n);
        cout << "Después del desplazamiento: A: " << A << ", Q: " << Q << ", Q_1: " << Q_1 << endl << endl;
        
        contador--;
    }
    
    //////////////////////////////////////////////////////////////////////
    /////////////// Interpretación de resultados finales /////////////////
    //////////////////////////////////////////////////////////////////////
    
    // Muestra de resultados
    cout << "=================================================================" << endl;
    cout << "Resultados:" << endl;
    cout << " " << endl;

    cout << "A: " << A << endl;
    cout << "Q: " << Q << endl;
    cout << "Multiplicando: " << multiplicando << endl;
    cout << "Multiplicador: " << multiplicador << endl;
    cout << "Resultado esperado: " << multiplicando * multiplicador << endl;

    cout << " " << endl;
    cout << "=================================================================" << endl;
    cout << " " << endl;

    cout << "Representación binaria (A:Q): " << A << Q << endl;
    int resultadoCalculado = multiplicando * multiplicador;
    cout << "Resultado calculado: " << resultadoCalculado << endl;

    cout << " " << endl;
    cout << "=================================================================" << endl;
}

int main() {
    int multiplicando, multiplicador, n;
    
    cout << " " << endl;
    cout << "Ingrese el multiplicando: ";
    cin >> multiplicando;
    cout << "Ingrese el multiplicador: ";
    cin >> multiplicador;
    cout << "Ingrese el número de bits (1-16): ";
    cin >> n;
    
    if (n <= 0 || n > 16) {
        cout << "El número de bits debe estar entre 1 y 16." << endl;
        return 1;
    }
    
    int maxValor = (1 << (n - 1)) - 1;
    int minValor = -(1 << (n - 1));

    algoritmoBooth(multiplicando, multiplicador, n);
    
    return 0;
}
