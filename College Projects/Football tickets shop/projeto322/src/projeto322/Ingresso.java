package projeto322;

import java.time.LocalDate;

public class Ingresso implements Pagavel{

    private Double preco;
    private Setor setorIngresso;
    private int assento;
    private LocalDate validade;
    private Partida partida;

    public Ingresso(Double preco, Setor setorIngresso, int assento, LocalDate validade, Partida partida) {
        if (preco <= 0) {
            throw new IllegalArgumentException("O preço deve ser um valor positivo");
        }

        if (!setorIngresso.estaDisponivel(assento)) {
            throw new IllegalArgumentException("Assento não está disponível!");
        }

        this.preco = preco;
        this.setorIngresso = setorIngresso;
        this.assento = assento;
        this.validade = validade;
        this.partida = partida;

        setorIngresso.ocuparAssento(assento);
    }

    @Override
    public Double getValor() {
        return this.preco;
    }

    @Override
    public void pagar() {
        if (!this.isIngressoValido()) {
            throw new IllegalArgumentException("Ingresso não é válido!");
        }
        this.invalidaIngresso();
    }

    public void invalidaIngresso() {
        this.validade = LocalDate.now().minusDays(1);
        this.setorIngresso.liberarAssento(this.assento);
    }

    public boolean isIngressoValido() {
        return LocalDate.now().isBefore(this.validade);
    }
    public Double getPreco() {
        return preco;
    }

    public Setor getSetorIngresso() {
        return setorIngresso;
    }

    public int getAssento() {
        return assento;
    }

    public LocalDate getValidade() {
        return validade;
    }

    public Partida getPartida() {
        return partida;
    }

    public void setPreco(Double preco) {
        this.preco = preco;
    }

    public void setSetorIngresso(Setor setorIngresso) {
        this.setorIngresso = setorIngresso;
    }

    public void setAssento(int assento) {
        this.assento = assento;
    }

    public void setValidade(LocalDate validade) {
        this.validade = validade;
    }

    public void setPartida(Partida partida) {
        this.partida = partida;
    }
}

