package projeto322;

import java.awt.FlowLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JTextField;

public class Cadastro {
    public UsuarioRegistrado usuario;
    private JTextField campoNome;
    private JTextField campoEmail;
    private JTextField campoCPF;
    private JTextField campoClube;
    private JTextField campoSenha;

    public Cadastro(){
    	
        JFrame janela = new JFrame("Cadastro");
        janela.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        janela.setSize(500, 500);
        /*Cria um campo para nome */
        campoNome = new JTextField(10);
		JLabel labelNome = new JLabel ("Nome: ");
		labelNome.setLabelFor(campoNome);

		/* Cria um campo de email */
		campoEmail = new JTextField(10);
		JLabel labelEmail = new JLabel ("Email: ");
		labelEmail.setLabelFor(campoEmail);

        /* Cria um campo de CPF */
        campoCPF = new JTextField(10);
        JLabel labelCPF = new JLabel("CPF: ");
        labelCPF.setLabelFor(campoCPF);

        /* Cria um campo de clube*/
        campoClube = new JTextField(10);
        JLabel labelClube = new JLabel("Clube: ");
        labelClube.setLabelFor(campoClube);

        /* Cria um campo de senha */
        campoSenha= new JTextField(10);
        JLabel labelSenha = new JLabel("Senha: ");
        labelSenha.setLabelFor(campoSenha);

        /* Cria um botao de confirma */
        JButton confima = new JButton("Confirmar");
        confima.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
            	String nome = campoNome.getText();
                String email = campoEmail.getText();
                String cpf = campoCPF.getText();
                String clube = campoClube.getText();
                String senha = campoSenha.getText();
                usuario = new UsuarioRegistrado(email, nome, senha, cpf);
                janela.setVisible(false);
                janela.dispose();
            }
        });

        /*Addciona as coisas na janela  */
        janela.getContentPane().setLayout(new FlowLayout());
        janela.add(labelNome);
        janela.add(campoNome);
        janela.add(labelCPF);
        janela.add(campoCPF);
        janela.add(labelClube);
        janela.add(campoClube);
        janela.add(labelEmail);
        janela.add(campoEmail);
        janela.add(labelSenha);
        janela.add(campoSenha);
        janela.add(confima);
        janela.setVisible(true);
    }

    public UsuarioRegistrado getUsuario(){
        return this.usuario;
    }
}
