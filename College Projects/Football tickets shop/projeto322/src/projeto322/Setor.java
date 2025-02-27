package projeto322;

import java.util.Collections;
import java.util.List;
import java.util.ArrayList;

public class Setor {

    private String nome;
    private int numeroDeCadeiras;
    private List<Boolean> assentos;

    public Setor(String nome, int numeroDeCadeiras) {
        if (nome == null || nome.trim().isEmpty()) {
            throw new IllegalArgumentException("O nome não pode ser vazio ou nulo");
        }

        this.nome = nome;
        this.numeroDeCadeiras = numeroDeCadeiras;
        this.assentos = new ArrayList<>(Collections.nCopies(numeroDeCadeiras, true));
    }

    public boolean estaDisponivel(int assento) {
        return this.assentos.get(assento - 1);
        }

    public void ocuparAssento(int assento) {
        if (!this.assentos.get(assento - 1)) {
            throw new IllegalArgumentException("Assento já está ocupado!");
            }
        this.assentos.set(assento - 1, false);
        }

    public void liberarAssento(int assento) {
        if (this.assentos.get(assento - 1)) {
            throw new IllegalArgumentException("Assento já está disponível!");
            }
        this.assentos.set(assento - 1, true);
        }

    public String getNome() {
        return this.nome;
    }

    public int getNumeroDeCadeiras() {
        return this.numeroDeCadeiras;
    }

    public List<Boolean> getAssentos() {
    	 return new ArrayList<>(this.assentos);
    }

    public void setNome(String nome) {
        this.nome = nome;
    }
}
