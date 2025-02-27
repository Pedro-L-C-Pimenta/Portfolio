package projeto322;

import javax.swing.*;
import java.awt.*;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.geom.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

// Representação gráfica de um estádio para efetuar a escolha dos setores
public class GUIestádio extends JPanel {
    private static final int WIDTH = 400;
    private static final int HEIGHT = 400;
    private static final Color DEFAULT_COLOR = Color.LIGHT_GRAY;
    private static final Color CLICKED_COLOR = Color.DARK_GRAY;
    private static final double[] PERCENTAGES = {25, 25, 20, 10, 20};
    // Nortão da massa, Leste cara pro Sol, Oeste dos corneteiros e 10% do setor visitante
    private GUIsetor[] setores;
    private static final String[] NAMES = {"Setor Norte", "Setor Oeste", "Setor Sul", 
    "Setor Visitante", "Setor Leste"};
    private String selectedSliceName;

    public GUIestádio() {
        setPreferredSize(new Dimension(WIDTH, HEIGHT));
        setBackground(Color.WHITE);

        setores = new GUIsetor[PERCENTAGES.length];
        double startAngle = 45;
        for (int i = 0; i < PERCENTAGES.length; i++) {
            double percentage = PERCENTAGES[i];
            double arcAngle = (percentage / 100) * 360;

            setores[i] = new GUIsetor(startAngle, arcAngle, DEFAULT_COLOR);
            startAngle += arcAngle;
        }

        addMouseListener(new MouseListener() {
            @Override
            public void mouseClicked(MouseEvent e) {
                for (GUIsetor setor : setores) {
                    if (setor.contains(e.getX(), e.getY())) {
                        setor.setClicked(true);
                        selectedSliceName = NAMES[getClickedSliceIndex(setor)];
                    } else {
                        setor.setClicked(false);
                    }
                }
                repaint();
            }


            @Override
            public void mousePressed(MouseEvent e) {
            }

            @Override
            public void mouseReleased(MouseEvent e) {
            }

            @Override
            public void mouseEntered(MouseEvent e) {
            }

            @Override
            public void mouseExited(MouseEvent e) {
            }
        });
    }

    private int getClickedSliceIndex(GUIsetor clickedSetor) {
        for (int i = 0; i < setores.length; i++) {
            if (setores[i] == clickedSetor) {
                return i;
            }
        }
        return -1;
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);

        Graphics2D g2d = (Graphics2D) g.create();
        g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);

        double totalAngle = 45;
        for (int i = 0; i < setores.length; i++) {
            GUIsetor setor = setores[i];
            double arcAngle = setor.getArcAngle();

            g2d.setColor(setor.isClicked() ? CLICKED_COLOR : setor.getColor());
            g2d.fill(new Arc2D.Double(0, 0, WIDTH, HEIGHT, totalAngle, arcAngle, Arc2D.PIE));

            g2d.setColor(Color.BLACK);
            g2d.draw(new Arc2D.Double(0, 0, WIDTH, HEIGHT, totalAngle, arcAngle, Arc2D.PIE));

            totalAngle += arcAngle;
        }


        // Desenha o buraco interno
        double innerCircleDiameter = Math.min(WIDTH, HEIGHT) / 1.5;
        double innerCircleX = (WIDTH - innerCircleDiameter) / 2;
        double innerCircleY = (HEIGHT - innerCircleDiameter) / 2;
        g2d.setColor(getBackground());
        g2d.fill(new Ellipse2D.Double(innerCircleX, innerCircleY, innerCircleDiameter, innerCircleDiameter));
        g2d.setColor(Color.BLACK);
        g2d.draw(new Ellipse2D.Double(innerCircleX, innerCircleY, innerCircleDiameter, innerCircleDiameter));

        // Escreve o nome da fatia selecionada
        if (selectedSliceName != null) {
            FontMetrics fontMetrics = g2d.getFontMetrics();
            int stringWidth = fontMetrics.stringWidth(selectedSliceName);
            int x = (WIDTH - stringWidth) / 2;
            int y = (HEIGHT - fontMetrics.getHeight()) / 2; // Center vertically
            Font originalFont = g2d.getFont();
            Font biggerFont = originalFont.deriveFont(Font.BOLD, 24); // Increase font size
            g2d.setFont(biggerFont);
            g2d.drawString(selectedSliceName, x, y);
        }

        g2d.dispose();
    }


    private static class GUIsetor {
        private double startAngle;
        private double arcAngle;
        private Color color;
        private boolean isClicked;

        public GUIsetor(double startAngle, double arcAngle, Color color) {
            this.startAngle = startAngle;
            this.arcAngle = arcAngle;
            this.color = color;
        }

        public double getArcAngle() {
            return arcAngle;
        }

        public Color getColor() {
            return color;
        }

        public boolean isClicked() {
            return isClicked;
        }

        public void setClicked(boolean clicked) {
            isClicked = clicked;
        }

        public boolean contains(int x, int y) {
            double centerX = WIDTH / 2.0;
            double centerY = HEIGHT / 2.0;
            double radius = Math.min(WIDTH, HEIGHT) / 2.0;

            double endAngle = startAngle + arcAngle;
            double innerRadius = radius / 1.5;

            Shape outerArc = new Arc2D.Double(centerX - radius, centerY - radius, radius * 2, radius * 2,
                    startAngle, arcAngle, Arc2D.PIE);
            Shape innerArc = new Arc2D.Double(centerX - innerRadius, centerY - innerRadius, innerRadius * 2,
                    innerRadius * 2, startAngle, arcAngle, Arc2D.PIE);
            Area area = new Area(outerArc);
            area.subtract(new Area(innerArc));

            return area.contains(x, y);
        }
    }

    // Para teste interno da GUI
    public static void main(String[] args) {
        SwingUtilities.invokeLater(GUIestádio::createAndShowGUI);
    }

    private static void createAndShowGUI() {
        JFrame frame = new JFrame("Estádio GUI");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

        GUIestádio estádioGUI = new GUIestádio();
        JPanel mainPanel = new JPanel();
        mainPanel.setLayout(new BorderLayout());
        mainPanel.add(estádioGUI, BorderLayout.CENTER);

        JButton confirmButton = new JButton("Confirmar");
        confirmButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                JOptionPane.showMessageDialog(frame, "Setor escolhido com sucesso!");
                System.exit(0);
            }
        });

        mainPanel.add(confirmButton, BorderLayout.SOUTH);

        frame.getContentPane().add(mainPanel);
        frame.pack();
        frame.setLocationRelativeTo(null);
        frame.setVisible(true);
    }


}

