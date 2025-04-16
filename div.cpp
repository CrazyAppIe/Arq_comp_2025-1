#include <iostream>
#include <bitset>
#include <cmath>
using namespace std;

//////////////////////////////////////////////////////////////////////
//////////////////// Structura de componentes ////////////////////////
//////////////////////////////////////////////////////////////////////

struct ComponentesFloat {
    bool signo; // 1 bit de signo
    unsigned int exponente; // 8 bits de exponente
    unsigned int significando; // 23 bits de significando
};

/////////////////////////////////////////////////////////////////////
//////////////// Función para extraer componente ////////////////////
/////////////////////////////////////////////////////////////////////

ComponentesFloat extraerComponentes(float numero) {

    // Union para acceder a los 32 bits que conforman la representación binaria del input para almacenarlas en variables externas

    union {
        float f;
        unsigned int bits;
    } unionBits;
    
    unionBits.f = numero;
    
    ComponentesFloat componentes;
    
    // Extracción de los componentes

    componentes.signo = (unionBits.bits >> 31) & 1; // Signo (bit 31)
    componentes.exponente = (unionBits.bits >> 23) & 0xFF; // Exponente (bits 23-30)
    componentes.significando = unionBits.bits & 0x7FFFFF; // Significante (bits 0-22)
    
    return componentes;
}

//////////////////////////////////////////////////////////////////////
//////////////// Función para combinar componente ////////////////////
//////////////////////////////////////////////////////////////////////

// Combinar componentes (crear float)

float combinarComponentes(ComponentesFloat componentes) {

    union {
        float f;
        unsigned int bits;
    } unionBits;
    
    // Ensamblaje (Representación IEEE 754)

    unionBits.bits = (componentes.signo << 31) | // Signo (bit 31)
             (componentes.exponente << 23) | // Exponente (bits 23-30)
             (componentes.significando & 0x7FFFFF); // Significante (bits 0-22)
    
    return unionBits.f;
}

// Función para mostrar el estado

void mostrarFloat(float numero) {
    ComponentesFloat componentes = extraerComponentes(numero);
    
    cout << "Valor: " << numero << endl;
    cout << "Signo: " << componentes.signo << endl;
    cout << "Exponente (biased): " << componentes.exponente << " (0x" << hex << componentes.exponente << dec << ")" << endl;
    cout << "Significando: 0x" << hex << componentes.significando << dec << endl;
    
    // Representación completa

    union {
        float f;
        unsigned int bits;
    } unionBits;

    unionBits.f = numero;
    
    cout << "Representación binaria: " << bitset<32>(unionBits.bits) << endl;
}

//////////////////////////////////////////////////////////////////////
/////////////////// Algoritmo de División ////////////////////////////
//////////////////////////////////////////////////////////////////////

float dividirFloat(float numerador, float denominador) {

    // Paso 1: Casos especiales

    // Div 0

    if (denominador == 0.0f) {

        cout << "Error: División por cero!" << endl;

        // Mayor o igual que 0 infinito, menor que 0 -infinito

        return (numerador >= 0) ? INFINITY : -INFINITY;
    }
    
    // 0/... (Siempre 0)

    if (numerador == 0.0f) {
        cout << "El numerador es cero, retornando 0" << endl;
        return 0.0f;
    }
    
    // Paso 1.5:  Extraer componentes

    ComponentesFloat componentesNumerador = extraerComponentes(numerador);
    ComponentesFloat componentesDenominador = extraerComponentes(denominador);
    
    cout << "\n=== Componentes del dividendo ===" << endl;
    mostrarFloat(numerador);
    
    cout << "\n=== Componentes del divisor ===" << endl;
    mostrarFloat(denominador);
    
    // Paso 2: Calcular el signo del resultado

    bool signoResultado = componentesNumerador.signo ^ componentesDenominador.signo;
    cout << "\n=== Cálculo del signo del resultado ===" << endl;
    cout << "Signo X XOR Signo Y: " << componentesNumerador.signo << " XOR " << componentesDenominador.signo << " = " << signoResultado << endl;
    
    // Paso 3: Restar los exponentes (y sumar el bias)
    
    // Bias de 32 bits igual a 127 (IEEE)

    int bias = 127;
    int exponenteResultado = (int)componentesNumerador.exponente - (int)componentesDenominador.exponente + bias;
    
    cout << "\n=== Cálculo del exponente del resultado ===" << endl;
    cout << "Exponente X - Exponente Y + Bias: " << componentesNumerador.exponente << " - " << componentesDenominador.exponente << " + " << bias << " = " << exponenteResultado << endl;
    
    // Overflow/Underflow

    if (exponenteResultado >= 255) {

        cout << "Error: Overflow del exponente!" << endl;
        return signoResultado ? -INFINITY : INFINITY;

    }
    
    if (exponenteResultado <= 0) {

        cout << "Error: Underflow del exponente!" << endl;
        return signoResultado ? -0.0f : 0.0f;

    }
    
    // Paso 4: Dividir los significandos

    // Se añade el '1' implícito al frente del significando (para números normalizados)
    
    unsigned int significandoNumerador = componentesNumerador.significando | 0x800000;  // Añadir el bit implícito '1'
    unsigned int significandoDenominador = componentesDenominador.significando | 0x800000;  // Añadir el bit implícito '1'
    
    cout << "\n=== Preparación de significandos con el bit implícito ===" << endl;
    cout << "Significando Numerador con bit implícito: 0x" << hex << significandoNumerador << " (" << bitset<24>(significandoNumerador) << ")" << dec << endl;
    cout << "Significando Denominador con bit implícito: 0x" << hex << significandoDenominador << " (" << bitset<24>(significandoDenominador) << ")" << dec << endl;
    
    // Aumentar la precisión, desplazamos el significando del dividendo (más bits)

    unsigned long long significandoNumeradorExtendido = static_cast<unsigned long long>(significandoNumerador) << 23;
    
    cout << "Significando Numerador extendido (desplazado 23 bits): 0x" << hex << significandoNumeradorExtendido << dec << endl;
    
    // Realizamos la división de los significandos
    unsigned long long significandoResultado = significandoNumeradorExtendido / significandoDenominador;
    
    cout << "\n=== División de significandos ===" << endl;
    cout << "Resultado de la división: 0x" << hex << significandoResultado << dec << endl;
    
    // Paso 5: Normalización

    // Desplamiento por medio de un contador para que el primer bit sea '1'

    int contadorDesplazamiento = 0;
    unsigned long long significandoTemporal = significandoResultado;
    
    while ((significandoTemporal & 0x800000) == 0 && significandoTemporal != 0) {
        significandoTemporal <<= 1;
        contadorDesplazamiento++;
        exponenteResultado--;
    }
    
    cout << "\n=== Normalización ===" << endl;
    cout << "Bits desplazados para normalizar: " << contadorDesplazamiento << endl;
    cout << "Exponente ajustado: " << exponenteResultado << endl;
    
    if (exponenteResultado <= 0) {
        cout << "Error: Underflow después de normalización!" << endl;
        return signoResultado ? -0.0f : 0.0f;
    }
    
    if (exponenteResultado >= 255) {
        cout << "Error: Overflow después de normalización!" << endl;
        return signoResultado ? -INFINITY : INFINITY;
    }
    
    // Paso 6: Redondeo

    unsigned int significandoFinal = (significandoTemporal >> 0) & 0x7FFFFF;
    
    cout << "\n=== Significando final (después de redondeo) ===" << endl;
    cout << "Significando final: 0x" << hex << significandoFinal << " (" << bitset<23>(significandoFinal) << ")" << dec << endl;
    
    // Paso 7: Ensamblar resultado

    ComponentesFloat componentesResultado;
    componentesResultado.signo = signoResultado;
    componentesResultado.exponente = exponenteResultado;
    componentesResultado.significando = significandoFinal;
    
    float resultado = combinarComponentes(componentesResultado);
    
    cout << "\n=== Resultado Final ===" << endl;
    mostrarFloat(resultado);
    
    return resultado;
}

int main() {

    float numerador; 
    float denominador;
    
    cout << "===== División en Punto Flotante =====" << endl;
    cout << "Ingrese el dividendo (X/...): ";
    cin >> numerador;
    cout << "Ingrese el divisor (.../Y): ";
    cin >> denominador;
    
    float divisionPuntoFlotante = dividirFloat(numerador, denominador);
    
    // Compaparar

    float divisionCompiladaC = numerador / denominador;
    
    cout << "\n=== Comparación de Resultados ===" << endl;
    cout << "Resultado implementación: " << divisionPuntoFlotante << endl;
    cout << "Resultado operación compilador de C++: " << divisionCompiladaC << endl;
    cout << "Diferencia: " << divisionPuntoFlotante - divisionCompiladaC << endl;
    
}