package projeto322;
import java.util.List;
import java.util.ArrayList;

public abstract class Usuario {

    protected String email;
    protected String nome;
    protected String senha;
    protected String cpf;
    protected Clube clube;
    protected Carteira carteira;
    protected Carrinho carrinho;
    protected List<Ingresso> ingressos;

    public Usuario(String email, String nome, String senha, String cpf) {
        if (!Validacao.validarEmail(email)) {
            throw new IllegalArgumentException("Email inválido!");
        }

        if (!Validacao.validarCPF(cpf)) {
            throw new IllegalArgumentException("CPF inválido!");
        }

        this.email = email;
        this.nome = nome;
        this.senha = senha;
        this.cpf = cpf;
        this.carteira = new Carteira();
        this.carrinho = new Carrinho();
        this.ingressos = new ArrayList<>();
    }

    public void alteraSenha(String senhaAtual, String novaSenha) {
        if (!this.senha.equals(senhaAtual)) {
            throw new IllegalArgumentException("A senha atual está incorreta!");
        }

        if (!Validacao.validarSenha(novaSenha)) {
            throw new IllegalArgumentException("A nova senha não é válida!");
        }

        this.senha = novaSenha;
    }

    public void addIngresso(Ingresso ingresso) {
        ingressos.add(ingresso);
    }

    public void removeIngresso(Ingresso ingresso) {
        ingressos.remove(ingresso);
    }

    public void alteraTime(Clube novoClube) {
        this.clube = novoClube;
    }

    public String getEmail() {
    	 return this.email;
    }

    public String getNome() {
        return this.nome;
    }

    public void setNome(String nome) {
        this.nome = nome;
    }

    public Clube getClube() {
        return this.clube;
    }

    public void setClube(Clube clube) {
        this.clube = clube;
    }

    public Carteira getCarteira() {
        return this.carteira;
    }

    public Carrinho getCarrinho() {
        return this.carrinho;
    }

    public List<Ingresso> getIngressos() {
        return this.ingressos;
    }

    public void setIngressos(List<Ingresso> ingressos) {
        this.ingressos = ingressos;
    }

    public String getCpf() {
        return this.cpf;
    }

}
