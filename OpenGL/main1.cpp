//#include "Angel.h"
//#include <cmath>
//
//const float PI = 3.14159265358979323846;
//
//void drawClockTicks() {
//    float radius = 0.9f;  // Bán kính của mặt đồng hồ
//    float tickLength;     // Độ dài vạch
//    int numTicks = 60;    // Tổng số vạch (12 vạch lớn, 48 vạch nhỏ)
//
//    glColor3f(0.0f, 0.0f, 0.0f);  // Màu đen cho vạch
//
//    for (int i = 0; i < numTicks; i++) {
//        // Xác định độ dài vạch (vạch lớn dài hơn)
//        if (i % 5 == 0) {
//            tickLength = 0.15f;  // Vạch lớn
//        }
//        else {
//            tickLength = 0.05f;  // Vạch nhỏ
//        }
//
//        // Tính toán vị trí điểm đầu và cuối của vạch
//        float angle = 2 * PI * i / numTicks;  // Góc tính theo radian
//        float xStart = radius * cos(angle);
//        float yStart = radius * sin(angle);
//        float xEnd = (radius - tickLength) * cos(angle);
//        float yEnd = (radius - tickLength) * sin(angle);
//
//        // Vẽ vạch
//        glBegin(GL_LINES);
//        glVertex2f(xStart, yStart);
//        glVertex2f(xEnd, yEnd);
//        glEnd();
//    }
//}
//
//void display() {
//    glClear(GL_COLOR_BUFFER_BIT);
//    glLoadIdentity();
//
//    // Vẽ mặt đồng hồ
//    drawClockTicks();
//
//    glutSwapBuffers();
//}
//
//void initOpenGL() {
//    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);  // Nền trắng
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);  // Thiết lập không gian 2D
//}
//
//int main(int argc, char** argv) {
//    glutInit(&argc, argv);
//    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
//    glutInitWindowSize(500, 500);
//    glutCreateWindow("Clock Ticks");
//
//    initOpenGL();
//
//    glutDisplayFunc(display);
//    glutMainLoop();
//
//    return 0;
//}
