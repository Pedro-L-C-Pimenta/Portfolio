package projeto322;

public class UsuarioRegistrado extends Usuario {

	public UsuarioRegistrado(String email, String nome, String senha, String cpf) {
        super(email, nome, senha, cpf);
    }
	
	public void atualizarEmail(String novoEmail) {
        if (!Validacao.validarEmail(novoEmail)) {
            throw new IllegalArgumentException("Email inválido!");
        }

        this.email = novoEmail;
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
}
