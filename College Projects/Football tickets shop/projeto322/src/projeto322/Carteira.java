package projeto322;

public class Carteira {

    private double saldo;

    public Carteira() {
        this.saldo = 0.0;
    }

    public void depositar(double valor) {
        if (valor <= 0) {
            throw new IllegalArgumentException("O valor de depósito deve ser positivo.");
        }
        this.saldo += valor;
    }

    public void sacar(double valor) {
        if (valor <= 0) {
            throw new IllegalArgumentException("O valor de saque deve ser positivo.");
        }
        if (valor > this.saldo) {
            throw new IllegalArgumentException("Saldo insuficiente na carteira.");
        }
        this.saldo -= valor;
    }

    public double getSaldo() {
        return this.saldo;
    }
}
