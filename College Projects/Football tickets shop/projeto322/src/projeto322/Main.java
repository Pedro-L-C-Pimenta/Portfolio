package projeto322;

import java.text.DateFormat;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.JFrame;
import javax.swing.JButton;
import javax.swing.JPanel;
import javax.swing.SwingUtilities;
import javax.swing.BoxLayout;

public class Main {
    // Variável para manter o controle do usuário registrado
    static UsuarioRegistrado usuarioAtual = null;

    public static void main(String[] args) throws ParseException {

        // Criação de Clubes no banco de dados
        Clube bahia = criarClube("Bahia", "Arena Fonte Nova");
        Clube botafogo = criarClube("Botafogo", "Nilton Santos");
        Clube fluminense = criarClube("Fluminense", "Maracanã");
        Clube gremio = criarClube("Grêmio", "Arena do Grêmio");
        Clube saoPaulo = criarClube("São Paulo", "Morumbi");
        Clube vasco = criarClube("Vasco da Gama", "São Januário");

        // Partidas já registradas
        DateFormat DFormat = new SimpleDateFormat("dd/MM/yy", Locale.getDefault());
        String julho1 = "01/07/2023";
        String julho2 = "02/07/2023";
        registrarPartida(saoPaulo, fluminense, DFormat, julho1, "mandante", "visitante");
        registrarPartida(bahia, gremio, DFormat, julho1, "mandante", "visitante");
        registrarPartida(botafogo, vasco, DFormat, julho2, "mandante", "visitante");

        // Registro de usuário
        try {
            registerUser("usuario@gmail.com", "User", "password", "12345678901");
            
            // Deposita dinheiro na carteira
            depositar(usuarioAtual, 200.0);
            
            // Imprime o saldo restante
            System.out.println("Saldo remanescente Usuario: " + usuarioAtual.getCarteira().getSaldo());
        
        } catch (IllegalArgumentException e) {
            System.out.println(e.getMessage());

            // Swing Utilities para garantir que a GUI seja criada na Event-Dispatching thread
            SwingUtilities.invokeLater(new Runnable() {
                public void run() {
                    createAndShowGUI();
                }
            });}
        }

        private static void createAndShowGUI() {
            JFrame frame = new JFrame("Ingressos e Cadastro");
            frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
            frame.setSize(400, 200);

            JPanel panel = new JPanel();
            panel.setLayout(new BoxLayout(panel, BoxLayout.PAGE_AXIS));

            // para comprar ingressos
            JButton ingressosButton = new JButton("Comprar Ingressos");
            ingressosButton.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    new SwingIngressos().createAndShowGUI();
                }
            });

            // para registrar um ususario
            JButton cadastroButton = new JButton("Cadastrar Usuario");
            cadastroButton.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    new Cadastro();
                }
            });

            // Adiciona ao layout principal
            panel.add(ingressosButton);
            panel.add(cadastroButton);

            
            frame.add(panel);

    
            frame.setVisible(true);
        }
        
        public static Clube criarClube(String nome, String estadio){
            return new Clube(nome, estadio);
        }
        
        public static void registrarPartida(Clube mandante, Clube visitante, DateFormat dateFormat, String data, String papelMandante, String papelVisitante) throws ParseException{
            mandante.adicionaPartida(dateFormat.parse(data), null, visitante, papelMandante);
            visitante.adicionaPartida(dateFormat.parse(data), null, mandante, papelVisitante);
        }

        public static void registerUser(String email, String nome, String senha, String cpf) {
            usuarioAtual = new UsuarioRegistrado(email, nome, senha, cpf);
        }

        public static void depositar(UsuarioRegistrado usuario, double valor) {
            usuario.getCarteira().depositar(valor);
        }
    }

