package projeto322;

import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.time.LocalTime;
import java.time.LocalDate;
import java.util.*;

public class SwingIngressos {
	
	private Ingresso ingressoSelecionado;

    public static void main(String[] args) {
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                new SwingIngressos().createAndShowGUI();
            }
        });
    }

    public void createAndShowGUI() {
        JFrame frame = new JFrame("Compra de Ingressos");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setSize(400, 400);

        JPanel panel = new JPanel();
        panel.setLayout(new BoxLayout(panel, BoxLayout.Y_AXIS));

        // Criação dos botões para escolha dos jogos
        JButton jogo1Button = createGameButton(new Partida(new Date(), LocalTime.now(), new Clube("Time A", "Estádio A"), new Clube("Time B", "Estádio B")));
        JButton jogo2Button = createGameButton(new Partida(new Date(), LocalTime.now(), new Clube("Time C", "Estádio C"), new Clube("Time D", "Estádio D")));
        JButton jogo3Button = createGameButton(new Partida(new Date(), LocalTime.now(), new Clube("Time E", "Estádio E"), new Clube("Time F", "Estádio F")));
        JButton jogo4Button = createGameButton(new Partida(new Date(), LocalTime.now(), new Clube("Time G", "Estádio G"), new Clube("Time H", "Estádio H")));

        // Criação do ComboBox para seleção do tipo de ingresso
        JComboBox<String> ingressoComboBox = new JComboBox<>(new String[]{"Inteira", "Meia-entrada"});
        ingressoComboBox.setSelectedIndex(0);

        // Criação do botão de pagamento
        JButton pagarButton = new JButton("Pagar");
        pagarButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                if (ingressoSelecionado != null) {
                    try {
                        ingressoSelecionado.pagar();
                        System.out.println("Pagamento realizado com sucesso!");
                    } catch (IllegalArgumentException ex) {
                        System.out.println("Erro ao realizar o pagamento: " + ex.getMessage());
                    }
                } else {
                    System.out.println("Selecione um ingresso antes de prosseguir com o pagamento.");
                }
            }
        });

        // Adicionando os componentes ao layout principal
        panel.add(jogo1Button);
        panel.add(jogo2Button);
        panel.add(jogo3Button);
        panel.add(jogo4Button);
        panel.add(ingressoComboBox);
        panel.add(pagarButton);

        // Adicionando o painel ao frame
        frame.add(panel);

        // Exibindo o frame
        frame.setVisible(true);
    }

    // Método auxiliar para criar os botões dos jogos
    private JButton createGameButton(Partida partida) {
        JButton button = new JButton("Jogo");
        button.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                Setor setor = new Setor("Setor A", 100); // Crie uma instância apropriada do Setor
                int assento = 1; // Defina o assento apropriado
                Double preco = 50.0; // Defina o preço apropriado

                try {
                    ingressoSelecionado = new Ingresso(preco, setor, assento, LocalDate.now().plusDays(1), partida);
                    System.out.println("Ingresso selecionado: " + partida.toString());
                } catch (IllegalArgumentException ex) {
                    System.out.println("Erro ao selecionar o ingresso: " + ex.getMessage());
                    ingressoSelecionado = null;
                }
            }
        });
        return button;
    }
}



