/*Chương trình chiếu sáng Blinn-Phong (Phong sua doi) cho hình lập phương đơn vị, điều khiển quay bằng phím x, y, z, X, Y, Z.*/

#include "Angel.h"  /* Angel.h là file tự phát triển (tác giả Prof. Angel), có chứa cả khai báo includes glew và freeglut*/
#include <ctime>

// remember to prototype
void generateGeometry(void);
void initGPUBuffers(void);
void shaderSetup(void);
void display(void);
void keyboard(unsigned char key, int x, int y);

typedef vec4 point4;
typedef vec4 color4;
using namespace std;

// Số các đỉnh của các tam giác
const int NumPoints = 36;

point4 points[NumPoints]; /* Danh sách các đỉnh của các tam giác cần vẽ*/
color4 colors[NumPoints]; /* Danh sách các màu tương ứng cho các đỉnh trên*/
vec3 normals[NumPoints]; /*Danh sách các vector pháp tuyến ứng với mỗi đỉnh*/

point4 vertices[8]; /* Danh sách 8 đỉnh của hình lập phương*/
color4 vertex_colors[8]; /*Danh sách các màu tương ứng cho 8 đỉnh hình lập phương*/

GLuint program;

GLfloat Theta[] = { 0, 0, 0,0,0,0,0 };
GLfloat theta[] = { 0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0 };
GLfloat i, dr = 5;

mat4 ctm, ctm_ban, ctm_bantb, ctm_cancau, ctm_xetai, ctm_xecau, ctm_xetai1, model, phongto;
GLuint model_loc;
mat4 projection;
GLuint projection_loc;
mat4 view;
GLuint view_loc;


void initCube()
{
	// Gán giá trị tọa độ vị trí cho các đỉnh của hình lập phương
	vertices[0] = point4(-0.5, -0.5, 0.5, 1.0);
	vertices[1] = point4(-0.5, 0.5, 0.5, 1.0);
	vertices[2] = point4(0.5, 0.5, 0.5, 1.0);
	vertices[3] = point4(0.5, -0.5, 0.5, 1.0);
	vertices[4] = point4(-0.5, -0.5, -0.5, 1.0);
	vertices[5] = point4(-0.5, 0.5, -0.5, 1.0);
	vertices[6] = point4(0.5, 0.5, -0.5, 1.0);
	vertices[7] = point4(0.5, -0.5, -0.5, 1.0);

	// Gán giá trị màu sắc cho các đỉnh của hình lập phương	
	vertex_colors[0] = color4(0.0, 0.0, 0.0, 1.0); // black
	vertex_colors[1] = color4(1.0, 0.0, 0.0, 1.0); // red
	vertex_colors[2] = color4(1.0, 1.0, 0.0, 1.0); // yellow
	vertex_colors[3] = color4(0.0, 1.0, 0.0, 1.0); // green
	vertex_colors[4] = color4(0.0, 0.0, 1.0, 1.0); // blue
	vertex_colors[5] = color4(1.0, 0.0, 1.0, 1.0); // magenta
	vertex_colors[6] = color4(1.0, 0.5, 0.0, 1.0); // orange
	vertex_colors[7] = color4(0.0, 1.0, 1.0, 1.0); // cyan
}
int Index = 0;
void quad(int a, int b, int c, int d)  /*Tạo một mặt hình lập phương = 2 tam giác, gán màu cho mỗi đỉnh tương ứng trong mảng colors*/
{
	vec4 u = vertices[b] - vertices[a];
	vec4 v = vertices[c] - vertices[b];
	vec3 normal = normalize(cross(u, v));

	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[b]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[d]; Index++;
}
void makeColorCube(void)  /* Sinh ra 12 tam giác: 36 đỉnh, 36 màu*/

{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}
void generateGeometry(void)
{
	initCube();
	makeColorCube();
}


void initGPUBuffers(void)
{
	// Tạo một VAO - vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Tạo và khởi tạo một buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors) + sizeof(normals), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), sizeof(normals), normals);


}

void shaderSetup(void)
{
	// Nạp các shader và sử dụng chương trình shader
	program = InitShader("vshader1.glsl", "fshader1.glsl");   // hàm InitShader khai báo trong Angel.h
	glUseProgram(program);

	// Khởi tạo thuộc tính vị trí đỉnh từ vertex shader
	GLuint loc_vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(loc_vPosition);
	glVertexAttribPointer(loc_vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint loc_vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc_vColor);
	glVertexAttribPointer(loc_vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	GLuint loc_vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(loc_vNormal);
	glVertexAttribPointer(loc_vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points) + sizeof(colors)));

	/* Khởi tạo các tham số chiếu sáng - tô bóng*/
	point4 light_position(0.0, 0.0, 1.0, 0.0);
	color4 light_ambient(0.2, 0.2, 0.2, 1.0);
	color4 light_diffuse(1.0, 1.0, 1.0, 1.0);
	color4 light_specular(1.0, 1.0, 1.0, 1.0);

	color4 material_ambient(1.0, 0.0, 1.0, 1.0);
	color4 material_diffuse(1.0, 0.8, 0.0, 1.0);
	color4 material_specular(1.0, 0.8, 0.0, 1.0);
	float material_shininess = 100.0;

	color4 ambient_product = light_ambient * material_ambient;
	color4 diffuse_product = light_diffuse * material_diffuse;
	color4 specular_product = light_specular * material_specular;

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);

	model_loc = glGetUniformLocation(program, "Model");
	projection_loc = glGetUniformLocation(program, "Projection");
	view_loc = glGetUniformLocation(program, "View");

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);        /* Thiết lập màu trắng là màu xóa màn hình*/
}
point4 light_position(0.0, 0.0, 1.0, 0.0);
color4 light_ambient(0.2, 0.2, 0.2, 1.0);
color4 light_diffuse(1.0, 1.0, 1.0, 1.0);
color4 light_specular(1.0, 1.0, 1.0, 1.0);
color4 material_ambient(1.0, 0.0, 1.0, 1.0);
color4 material_diffuse(0.0, 1.0, 0.0, 0);
color4 material_specular(1.0, 0.8, 0.0, 1.0);
float material_shininess = 100.0;
color4 ambient_product;
color4 diffuse_product;
color4 specular_product;
color4 t;
void setMau(float ad, float bd, float cd) {
	material_diffuse = vec4(ad, bd, cd, 1.0);
	ambient_product = light_ambient * material_ambient;
	diffuse_product = light_diffuse * material_diffuse;
	specular_product = light_specular * material_specular;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
}
void tualung() {
	setMau(0, 0, 0);
	mat4 instance = RotateX(-5) * Translate(0, 0.2, -0.14) * Scale(0.3, 0.25, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void tru() {
	setMau(0, 0, 0);
	mat4 instance = Translate(0, 0.035, -0.14) * Scale(0.02, 0.1, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void tayvinngang(GLfloat a, GLfloat b, GLfloat c) {
	setMau(0, 255, 0);
	mat4 instance = Translate(a, b, c) * Scale(0.02, 0.02, 0.15);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void tayvindoc(GLfloat a, GLfloat b, GLfloat c) {
	setMau(0, 255, 0);
	mat4 instance = Translate(a, b, c) * Scale(0.02, 0.17, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void matghe() {
	setMau(0, 0, 0);
	mat4 instance = Scale(0.3, 0.02, 0.3);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void changhe() {
	setMau(0, 0, 0);
	mat4 instance = Translate(0, -0.06, 0) * Scale(0.02, 0.12, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void chanduoi1() {
	setMau(0, 0, 0);
	mat4 instance = RotateX(40) * Translate(0, -0.135, 0.06) * Scale(0.02, 0.08, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void chanduoi2() {
	setMau(0, 0, 0);
	mat4 instance = RotateX(-40) * Translate(0, -0.135, -0.06) * Scale(0.02, 0.08, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void chanduoi3() {
	setMau(0, 0, 0);
	mat4 instance = RotateZ(-40) * Translate(0.06, -0.135, 0) * Scale(0.02, 0.08, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void chanduoi4() {
	setMau(0, 0, 0);
	mat4 instance = RotateZ(40) * Translate(-0.06, -0.135, 0) * Scale(0.02, 0.08, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void ghe() {
	mat4 vitri = Translate(-2, -1, -2.5);
	phongto = Scale(3, 3, 3);
	ctm = vitri * phongto;
	changhe();
	chanduoi1();
	chanduoi2();
	chanduoi3();
	chanduoi4();
	ctm = vitri * phongto * RotateX(Theta[0]) * RotateY(Theta[1]) * Translate(0, i, 0);
	tualung();
	tru();
	matghe();
	tayvinngang(0.14, 0.155, -0.075);
	tayvinngang(-0.14, 0.155, -0.075);
	tayvindoc(0.14, 0.08, 0);
	tayvindoc(-0.14, 0.08, 0);
}
void matban() {
	setMau(0, 0, 0);
	mat4 instance = Translate(0, 0, 0) * Scale(0.6, 0.02, 0.3);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_ban * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void chanban(GLfloat a, GLfloat b, GLfloat c) {
	setMau(0, 0, 0);
	mat4 instance = Translate(a, b, c) * Scale(0.03, 0.3, 0.03);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_ban * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void demt() {
	setMau(0, 255, 255);
	mat4 instance = Translate(0, 0.015, 0) * Scale(0.15, 0.03, 0.06);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_ban * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void trumt() {
	setMau(0, 255, 255);
	mat4 instance = Translate(0, 0.045, 0) * Scale(0.03, 0.06, 0.03);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_ban * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void manhinhmt() {
	setMau(0, 255, 255);
	mat4 instance = RotateX(5) * Translate(0, 0.19, 0) * Scale(0.3, 0.25, 0.03);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_ban * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void ongbut() {
	setMau(255, 0, 255);
	mat4 instance = Translate(0.25, 0.05, 0.1) * Scale(0.05, 0.1, 0.05);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_ban * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void but1() {
	setMau(255, 0, 0);
	mat4 instance = RotateX(10) * Translate(0.25, 0.15, 0.1) * Scale(0.01, 0.1, 0.01);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_ban * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void but2() {
	setMau(0, 255, 255);
	mat4 instance = RotateX(-10) * Translate(0.25, 0.1, 0.1) * Scale(0.01, 0.13, 0.01);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_ban * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void but3() {
	setMau(0, 255, 0);
	mat4 instance = RotateX(10) * Translate(0.24, 0.1, 0.1) * Scale(0.01, 0.13, 0.01);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_ban * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void chuot() {
	setMau(255, 0, 255);
	mat4 instance = Translate(-0.24, 0.01, 0) * Scale(0.03, 0.03, 0.05);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_ban * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void banthungan() {
	mat4 vitri = Translate(-2, -1.5, -1);
	ctm_ban = vitri * phongto;
	matban();
	chanban(0.285, -0.15, -0.135);
	chanban(-0.285, -0.15, 0.135);
	chanban(-0.285, -0.15, -0.135);
	chanban(0.285, -0.15, 0.135);
	demt();
	trumt();
	manhinhmt();
	ongbut();
	but1();
	but2();
	but3();
	chuot();
}
void matbantb() {
	setMau(0, 0, 0);
	mat4 instance = Scale(0.7, 0.02, 0.4);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_bantb * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void chanbantb(GLfloat a, GLfloat b, GLfloat c) {
	setMau(0, 0, 0);
	mat4 instance = Translate(a, b, c) * Scale(0.03, 0.3, 0.03);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_bantb * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void bantrungbay() {
	mat4 vitri = Translate(2, -1, -1);
	ctm_bantb = vitri * phongto;
	matbantb();
	chanbantb(0.335, -0.15, 0.185);
	chanbantb(0.335, -0.15, -0.185);
	chanbantb(-0.335, -0.15, -0.185);
	chanbantb(-0.335, -0.15, 0.185);
}
void banhxe(GLfloat a, GLfloat b, GLfloat c) {
	setMau(255, 255, 0);
	for (int z = 0; z < 360; z += 1) {
		mat4 instance = RotateX(90) * Translate(a, b, c) * RotateY(z) * Scale(0.1, 0.05, 0.1);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_xetai1 * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
	}
}
void trucxe(GLfloat a, GLfloat b, GLfloat c) {
	setMau(0, 255, 0);
	mat4 instance = Translate(a, b, c) * Scale(0.05, 0.05, 0.4);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_xetai1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void thungxe() {
	setMau(0, 255, 0);
	mat4 instance = Translate(0.15, 0.05, 0.2) * Scale(0.6, 0.1, 0.4);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_xetai1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void mat1(GLfloat a, GLfloat b, GLfloat c) {
	setMau(0, 0, 255);
	mat4 instance = Translate(a, b, c) * Scale(0.02, 0.2, 0.4);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_xetai1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void matday() {
	setMau(0, 0, 255);
	mat4 instance = Translate(0.325, 0.11, 0.2) * Scale(0.25, 0.02, 0.39);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_xetai1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void matben(GLfloat a, GLfloat b, GLfloat c) {
	setMau(0, 0, 255);
	mat4 instance = Translate(a, b, c) * Scale(0.22, 0.17, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_xetai1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void matcua(GLfloat a, GLfloat b, GLfloat c) {
	setMau(255, 0, 255);
	mat4 instance = Translate(a, b, c) * Scale(0.02, 0.2, 0.39);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_xetai1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void cuatrai(GLfloat a, GLfloat b, GLfloat c) {
	setMau(255, 0, 255);
	mat4 instance = Translate(a, b, c) * Scale(0.26, 0.2, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_xetai1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void cuaphai(GLfloat a, GLfloat b, GLfloat c) {
	setMau(255, 0, 255);
	mat4 instance = Translate(a, b, c) * Scale(0.26, 0.2, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_xetai1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void mattren() {
	setMau(255, 0, 255);
	mat4 instance = Translate(0.02, 0.25, 0.2) * Scale(0.28, 0.02, 0.4);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_xetai1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
GLfloat m = 0, n;

void xetai() {
	mat4 vitri = Translate(2, -0.8, -1);
	ctm_xetai1 = vitri * Translate(m, 0, 0);
	banhxe(0, 0, 0);
	banhxe(0.3, 0, 0);
	banhxe(0, 0.4, 0);
	banhxe(0.3, 0.4, 0);
	trucxe(0, 0, 0.2);
	trucxe(0.3, 0, 0.2);
	thungxe();
	matben(0.31, 0.17, 0);
	matben(0.31, 0.17, 0.4);
	matday();
	mat1(0.2, 0.15, 0.2);
	mat1(0.43, 0.15, 0.2);
	mat1(0.15, 0.15, 0.2);
	matcua(-0.12, 0.15, 0.2);
	cuatrai(0.02, 0.15, 0);
	mattren();
	ctm_xetai1 = ctm_xetai1 * Translate(-0.14, 0, 0.4) * RotateY(Theta[3]);
	cuaphai(0.15, 0.14, -0.01);
}
void banhxecau(GLfloat a, GLfloat b, GLfloat c) {
	for (int z = 0; z < 360; z += 1) {
		mat4 instance = RotateX(90) * Translate(a, b, c) * RotateY(z) * Scale(0.1, 0.05, 0.1);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_xecau * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
	}
}
void thanxecau() {
	setMau(0, 255, 0);
	mat4 instance = Translate(0.15, 0.1, 0.2) * Scale(0.6, 0.15, 0.4);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_xecau * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void dauxecau() {
	setMau(0, 0, 255);
	mat4 instance = Translate(-0.01, 0.2, 0.2) * Scale(0.25, 0.2, 0.4);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_xecau * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void duoixecau() {
	setMau(255, 255, 0);
	mat4 instance = Translate(0, 0.3, 0) * Scale(0.3, 0.23, 0.4);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_xecau * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void truccau() {
	setMau(0, 255, 0);
	mat4 instance = RotateZ(-15) * Translate(0, 0.62, 0) * Scale(0.09, 0.4, 0.09);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_xecau * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void khucnoi() {
	setMau(255, 0, 0);
	mat4 instance = RotateZ(-20) * Translate(-0.01, 0.18, 0) * Scale(0.06, 0.4, 0.06);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_xecau * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}

void day() {
	setMau(0, 0, 0);
	mat4 instance = Translate(0.14, 0.18, 0) * Scale(0.02, 0.4, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_xecau * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void cuchang() {
	setMau(0, 0, 255);
	mat4 instance = Translate(0.14, 0, 0) * Scale(0.07, 0.07, 0.07);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_xecau * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void cancau() {
	mat4 vitri = Translate(1, -0.8, -0.5);
	ctm_xecau = vitri;
	banhxecau(0, 0, 0);
	banhxecau(0.3, 0, 0);
	banhxecau(0, 0.4, 0);
	banhxecau(0.3, 0.4, 0);
	thanxecau();
	dauxecau();
	ctm_xecau = ctm_xecau * Translate(0.3, 0, 0.2) * RotateY(Theta[4]);
	duoixecau();
	truccau();
	ctm_xecau = ctm_xecau * Translate(0.22, 0.78, 0) * RotateZ(Theta[5]);
	khucnoi();
	day();
	cuchang();

}
/* LÊ THị TRANG - ROBOT*/
void torso()
{
	setMau(0, 0, 255);
	mat4 instance = Translate(0, -0.35, 0) * Scale(0.6, 0.7, 0.3);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void head()
{
	setMau(0, 0, 0);
	mat4 instance = Translate(0, 0.075, 0) * Scale(0.2, 0.15, 0.3);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void upper_arm()
{
	setMau(0, 255, 255);
	mat4 instance = Scale(0.1, 0.3, 0.2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void lower_arm()
{
	setMau(0, 255, 255);
	mat4 instance = Scale(0.05, 0.3, 0.1);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void upper_leg()
{
	setMau(0, 255, 255);
	mat4 instance = Scale(0.2, 0.45, 0.25);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void lower_leg()
{
	setMau(0, 255, 255);
	mat4 instance = Scale(0.15, 0.35, 0.2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
mat4 trans, thunho;

void robot()
{
	mat4 vitri = Translate(-0.5, -1.5, -2);

	model = vitri * Translate(0, 0.75, 0) * RotateY(theta[0]);
	trans = model;
	torso();
	model = trans * RotateY(theta[1]);
	head();
	model = trans * Translate(0, -0.05, 0) * RotateX(theta[2]) * Translate(-0.35, -0.15, 0);
	upper_arm();
	model = model * Translate(0, -0.15, 0) * RotateX(theta[3]) * Translate(0, -0.15, 0);
	lower_arm();
	model = trans * Translate(0, -0.05, 0) * RotateX(theta[4]) * Translate(0.35, -0.15, 0);
	upper_arm();
	model = model * Translate(0, -0.15, 0) * RotateX(theta[5]) * Translate(0, -0.15, 0);
	lower_arm();
	model = trans * Translate(0, -0.7, 0) * RotateX(theta[6]) * Translate(-0.15, -0.225, 0);
	upper_leg();
	model = model * Translate(0, -0.225, 0) * RotateX(theta[7]) * Translate(0, -0.175, 0);
	lower_leg();
	model = trans * Translate(0, -0.7, 0) * RotateX(theta[8]) * Translate(0.15, -0.225, 0);
	upper_leg();
	model = model * Translate(0, -0.225, 0) * RotateX(theta[9]) * Translate(0, -0.175, 0);
	lower_leg();
}
/* END - ROBOT*/


/* LÊ THẢO VÂN - TỦ TRƯNG BÀY*/
mat4 ctm_oto;
mat4 ctm_tu;
float ctm_tu_delta_y = -0.1;
void banh(GLfloat a, GLfloat b, GLfloat c) {
	setMau(0, 0, 255);
	for (int z = 0; z < 360; z += 1) {
		mat4 instance = RotateX(90) * Translate(a, b, c) * RotateY(z) * Scale(0.09, 0.05, 0.09);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_oto * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
	}
}
void dayoto() {
	setMau(0, 255, 0);
	mat4 instance = Translate(0.15, 0.1, 0.15) * Scale(0.4, 0.1, 0.3);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_oto * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void dauoto() {
	setMau(255, 255, 0);
	mat4 instance = Translate(0.02, 0.175, 0.15) * Scale(0.15, 0.15, 0.3);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_oto * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void duoioto() {
	setMau(0, 255, 255);
	mat4 instance = Translate(0.24, 0.175, 0.15) * Scale(0.2, 0.25, 0.3);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_oto * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void oto1() {
	mat4 vitri = Translate(0.2, 0.18 + ctm_tu_delta_y, -0.15);
	mat4 thunho = vitri * Scale(0.5, 0.5, 0.5);
	ctm_oto = thunho * RotateX(Theta[6]);
	banh(0, 0, 0);
	banh(0.25, 0, 0);
	banh(0.25, 0.3, 0);
	banh(0, 0.3, 0);
	dayoto();
	dauoto();
	duoioto();
}
void oto2() {
	mat4 vitri = Translate(0.7, 0.18 + ctm_tu_delta_y, -0.15);
	mat4 thunho = vitri * Scale(0.5, 0.5, 0.5);
	ctm_oto = thunho * RotateX(Theta[6]);
	banh(0, 0, 0);
	banh(0.25, 0, 0);
	banh(0.25, 0.3, 0);
	banh(0, 0.3, 0);
	dayoto();
	dauoto();
	duoioto();
}
void oto3() {
	mat4 vitri = Translate(0.7, 1.05 + ctm_tu_delta_y, -0.15);
	mat4 thunho = vitri * Scale(0.5, 0.5, 0.5);
	ctm_oto = thunho * RotateX(Theta[6]);
	banh(0, 0, 0);
	banh(0.25, 0, 0);
	banh(0.25, 0.3, 0);
	banh(0, 0.3, 0);
	dayoto();
	dauoto();
	duoioto();
}
void oto4() {
	mat4 vitri = Translate(0.2, 1.05 + ctm_tu_delta_y, -0.15);
	mat4 thunho = vitri * Scale(0.5, 0.5, 0.5);
	ctm_oto = thunho * RotateX(Theta[6]);
	banh(0, 0, 0);
	banh(0.25, 0, 0);
	banh(0.25, 0.3, 0);
	banh(0, 0.3, 0);
	dayoto();
	dauoto();
	duoioto();
}
void mattraitu(GLfloat a, GLfloat b, GLfloat c) {
	setMau(0.1, 0.5, 0.2);
	mat4 instance = Translate(a, b, c) * Scale(0.03, 1, 0.4);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_tu * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void mattrentu(GLfloat a, GLfloat b, GLfloat c) {
	setMau(0, 0, 255);
	mat4 instance = Translate(a, b, c) * Scale(0.6, 0.03, 0.4);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_tu * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}
void ke(GLfloat a, GLfloat b, GLfloat c) {
	setMau(0, 0, 0);
	mat4 instance = Translate(a, b, c) * Scale(0.2, 0.05, 0.1);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ctm_tu * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);    /*Vẽ các tam giác*/
}

void tu() {
	mat4 vitri = Translate(0, 0.55 + ctm_tu_delta_y, 1.5);
	ctm_tu = vitri * RotateY(Theta[2]);
	mattraitu(0, 0, 0);
	mattraitu(0.6, 0, 0);
	mattraitu(0.3, 0, 0);
	mattrentu(0.3, 0, 0);
	mattrentu(0.3, 0.485, 0);
	mattrentu(0.3, -0.485, 0);
	ke(0.15, 0.03, -0.15);
	ke(0.45, 0.03, -0.15);
	ke(0.45, -0.45, -0.15);
	ke(0.15, -0.45, -0.15);
	oto1();
	oto2();
	oto3();
	oto4();
}

void tu2() {
	mat4 vitri = Translate(-0.7, 0.55 + ctm_tu_delta_y, 1.5);
	ctm_tu = vitri * RotateY(Theta[2]);
	mattraitu(0, 0, 0);
	mattraitu(0.6, 0, 0);
	mattraitu(0.3, 0, 0);
	mattrentu(0.3, 0, 0);
	mattrentu(0.3, 0.485, 0);
	mattrentu(0.3, -0.485, 0);
	ke(0.15, 0.03, -0.15);
	ke(0.45, 0.03, -0.15);
	ke(0.45, -0.45, -0.15);
	ke(0.15, -0.45, -0.15);
}
/* END TỦ TRUNG BÀY*/


/*TÊN LỬA*/
double cao_than = 2, rong_than = 0.5;
double dai_canh = 0.5, rong_canh = 0.3, day_canh = 0.02;
double de = 1;
mat4 model1;
void ten_lua() {
	setMau(0, 0, 255);
	//than ten lua
	mat4 instance = Translate(0, cao_than / 2, 0) * Scale(rong_than, cao_than, rong_than);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//dau ten lua
	instance = Translate(0, cao_than, 0) * Scale(rong_than / 1.4, rong_than / 1.4, rong_than) * RotateZ(45);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//canh trai
	setMau(0, 255, 255);
	instance = Translate(rong_than - rong_canh / 2, dai_canh / 2, 0) * Scale(rong_canh, dai_canh, day_canh);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//canh phai
	setMau(0, 255, 255);
	instance = Translate(-rong_than + rong_canh / 2, dai_canh / 2, 0) * Scale(rong_canh, dai_canh, day_canh);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//canh trai
	setMau(0, 255, 255);
	instance = RotateY(90) * Translate(rong_than - rong_canh / 2, dai_canh / 2, 0) * Scale(rong_canh, dai_canh, day_canh);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//canh phai
	setMau(0, 255, 255);
	instance = RotateY(90) * Translate(-rong_than + rong_canh / 2, dai_canh / 2, 0) * Scale(rong_canh, dai_canh, day_canh);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void de_be_phong_nho() {
	setMau(0, 0, 0);
	mat4 instance = Translate(0, de / 40, 0) * Scale(de / 2, de / 20, de / 2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void de_be_phong_to() {
	setMau(0, 255, 0);
	mat4 instance = Translate(0, 3 * de / 40, 0) * Scale(cao_than, de / 10, de);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
double yy = 0;
double alpha = 0, beta = 0, gama = 0;
void gia_do_ten_lua() {
	setMau(255, 255, 0);
	mat4 instance = Scale(cao_than + 1.5 * rong_canh, de / 5, de);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(cao_than / 2 - rong_than / 1.5, de / 5, 0) * Scale(de / 20, rong_canh, 3 * de / 5);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void le_be() {
	mat4 instance = Scale(0.01, 0.01, de);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void be_phong() {
	mat4 vitri = Translate(0.2, -1, -1);
	mat4 thunho = Scale(0.25, 0.25, 0.25);
	model1 = vitri * thunho * Translate(0, -0.6, 0);
	de_be_phong_nho();
	model1 *= RotateY(alpha);
	de_be_phong_to();
	model1 *= Translate(-cao_than / 2, 3 * de / 40 + de / 10, 0) * RotateZ(beta);
	le_be();
	model1 *= Translate((cao_than + 1.5 * rong_canh) / 2, 0, 0);
	gia_do_ten_lua();
	model *= RotateZ(-90 + gama) * Translate(-rong_than / 2 - rong_canh, -(cao_than + 1.5 * rong_canh) / 2 + yy, 0);
	ten_lua();

}

void spinArm(void)
{
	theta[2] += 0.01;
	if (theta[2] > 60) theta[2] -= 120;
	theta[4] -= 0.01;
	if (theta[4] < -60) theta[4] += 120;
	theta[6] += 0.01;
	if (theta[6] > 30) theta[6] -= 60;
	theta[7] = 0;
	theta[8] -= 0.01;
	if (theta[8] < -30) theta[8] += 60;
	theta[9] = 0;
	glutPostRedisplay();
}
void sit(void)
{
	theta[6] = 90;
	theta[7] = -90;
	theta[8] = 90;
	theta[9] = -90;
	glutPostRedisplay();
}
void unsit(void)
{
	theta[2] = 0;
	theta[4] = 0;
	theta[6] = 0;
	theta[7] = 0;
	theta[8] = 0;
	theta[9] = 0;
	glutPostRedisplay();
}

// BHV

GLfloat fanRotate = 0;
bool isRorate = true;
float quat_y = -0.3;
void fan_display() {
	setMau(0.4, 0.1, 0.01);
	mat4 instance = Translate(0, 2 + quat_y, 0.08);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, instance * Scale(0.1, 0.3 + quat_y, 0.1));
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	if (isRorate) {
		fanRotate += 10;
	}

	setMau(0.01, 0.5, 0.9);
	float scale = 0.4;
	instance = Translate(cos(fanRotate * DegreesToRadians) * scale, 2 + quat_y, sin(fanRotate * DegreesToRadians) * scale) * RotateY(-fanRotate);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, instance * Scale(1, 0.15, 0.1));
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	GLfloat fanRotate1 = fanRotate + 90;
	instance = Translate(cos(fanRotate1 * DegreesToRadians) * scale, 2 + quat_y, sin(fanRotate1 * DegreesToRadians) * scale) * RotateY(-fanRotate1);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, instance * Scale(1, 0.15, 0.1));
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	fanRotate1 += 90;
	instance = Translate(cos(fanRotate1 * DegreesToRadians) * scale, 2 + quat_y, sin(fanRotate1 * DegreesToRadians) * scale) * RotateY(-fanRotate1);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, instance * Scale(1, 0.15, 0.1));
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	fanRotate1 += 90;
	instance = Translate(cos(fanRotate1 * DegreesToRadians) * scale, 2 + quat_y, sin(fanRotate1 * DegreesToRadians) * scale) * RotateY(-fanRotate1);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, instance * Scale(1, 0.15, 0.1));
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	glutPostRedisplay();
}


float dong_ho_x = 0, dong_ho_y = -0.5;

void dong_ho()
{
	mat4 model_dong_ho = Translate(1 + dong_ho_x, 1 + dong_ho_y, 0) * Scale(1, 1, 1);
	setMau(0.3, 0.4, 0.5);
	mat4 instance = model_dong_ho * Translate(1 + dong_ho_x, 1 + dong_ho_y, 0);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, instance * Scale(1, 1, 0.1));
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	float scale = 0.4;

	for (int i = 0; i < 12; i++)
	{
		float angle = i * 30 * DegreesToRadians;
		setMau(0.8, 0.4, 0.5);
		instance = model_dong_ho * Translate(sin(angle) * scale, cos(angle) * scale, 0) * RotateZ(i * -30);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, model_dong_ho * instance * Scale(0.01, 0.1, 0.2));
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}


	time_t now = time(0);
	tm* localTime = localtime(&now);

	int hours = localTime->tm_hour % 12;  // Lấy giờ, giới hạn từ 0 đến 11
	int minutes = localTime->tm_min;     // Lấy phút
	int seconds = localTime->tm_sec;     // Lấy giây

	// Tính góc quay cho từng kim
	float hourAngle = 30.0f * hours - (minutes / 2.0f);  // 360° / 12 = 30° mỗi giờ
	float minuteAngle = 6.0f * minutes;                 // 360° / 60 = 6° mỗi phút
	float secondAngle = 6.0f * seconds;

	scale = 0.4;
	instance = model_dong_ho * Translate(sin(secondAngle * DegreesToRadians) * 0.1, cos(secondAngle * DegreesToRadians) * 0.1, 0) * RotateZ(-secondAngle);
	setMau(0.7, 0.4, 0.0);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model_dong_ho * instance * Scale(0.01, 0.5, 0.2));
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	scale = 0.4;
	instance = model_dong_ho * Translate(sin(minuteAngle * DegreesToRadians) * 0.1, cos(minuteAngle * DegreesToRadians) * 0.1, 0) * RotateZ(-minuteAngle);
	setMau(0.1, 0.7, 0.5);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model_dong_ho * instance * Scale(0.01, 0.4, 0.2));
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	scale = 0.4;
	instance = model_dong_ho * Translate(sin(hourAngle * DegreesToRadians) * 0.1, cos(hourAngle * DegreesToRadians) * 0.1, 0) * RotateZ(-hourAngle);
	setMau(0.1, 0.0, 0.8);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model_dong_ho * instance * Scale(0.01, 0.4, 0.2));
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	glutPostRedisplay();
}


void tuongBenTrai() {
	setMau(0.8, 0.8, 0.8); // Màu xám nhạt
	mat4 instance = Translate(-4.0, 0.5, 0) * Scale(0.05, 5.0, 5.0);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void tuongPhiaSau() {
	setMau(0.7, 0.7, 0.7); // Màu xám đậm hơn
	mat4 instance = Translate(0.0, 0.5, -2.0) * Scale(10.0, 5.0, 0.05);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void tuongBenTren() {
	setMau(0.9, 0.9, 0.9); // Màu sáng hơn
	mat4 instance = Translate(0.0, 3.0, 0.0) * Scale(10.0, 0.05, 5.0);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void tuong() {
	tuongBenTrai();
	tuongPhiaSau();
	tuongBenTren();
	glutPostRedisplay();
}


void display(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ghe();
	banthungan();
	bantrungbay();
	xetai();
	cancau();
	robot();
	be_phong();
	tu();
	tu2();
	fan_display();
	dong_ho();
	tuong();
	const vec3 viewer_pos(0.0, 0.0, 2.0);  /*Trùng với eye của camera*/
	glutSwapBuffers();

}
float cam_x = 0, cam_y = 0;
void reshape(int width, int height)
{
	vec4 eye(0, 0, 4, 1);
	vec4 at(0, 0, 0, 1);
	vec4 up(0, 3, 0, 3);

	view = LookAt(eye, at, up);
	glUniformMatrix4fv(view_loc, 1, GL_TRUE, view);

	projection = Frustum(-2, 2, -2, 2, 1, 100);
	glUniformMatrix4fv(projection_loc, 1, GL_TRUE, projection);

	glViewport(0, 0, width, height);
}
// r đó kiểu mà để chỗ vẽ tầng 2 í hoặc góc nhìn như này được k

void update_camera()
{
	vec4 eye(0, 0 + cam_y, 4 + cam_x, 1);
	vec4 at(0, 0, 0, 1);
	vec4 up(0, 1, 0, 1);

	view = LookAt(eye, at, up);
	glUniformMatrix4fv(view_loc, 1, GL_TRUE, view);

	projection = Frustum(-2, 2, -2, 2, 1, 100);
	glUniformMatrix4fv(projection_loc, 1, GL_TRUE, projection);
}

void keyboard(unsigned char key, int x, int y)
{
	// keyboard handler
	//cout << "key " << key << endl;
	switch (key) {
	case 033:			// 033 is Escape key octal value
		exit(1);		// quit program
		break;
	case 'a':
		Theta[0] += 5;
		glutPostRedisplay();
		break;
	case 's':
		Theta[1] += 5;
		glutPostRedisplay();
		break;
	case 'd':
		Theta[4] += 5;
		glutPostRedisplay();
		break;
	case 'f':
		Theta[5] -= 5;
		glutPostRedisplay();
		break;
	case '2':
		Theta[6] -= 5;
		glutPostRedisplay();
		break;
	case 'h':
		if (i < 0.02)
			i += 0.01;
		glutPostRedisplay();
		break;
	case 'H':
		if (i > 0)
			i -= 0.01;
		glutPostRedisplay();
		break;
	case 'g':
		if ((Theta[3] >= -90)) {
			Theta[3] -= 5;
		}
		glutPostRedisplay();
		break;
	case 'G':
		if ((Theta[3] < 0)) {
			Theta[3] += 5;
		}
		glutPostRedisplay();
		break;
	case 'j':
		if (m < 0.5)
			m += 0.01;
		glutPostRedisplay();
		break;
	case 'J':
		if (m > 0)
			m -= 0.01;
		glutPostRedisplay();
		break;
		/*---------------------- -*/
	case 'z':
		theta[0] += 5;
		if (theta[0] > 360) theta[0] -= 360;
		glutPostRedisplay();
		break;
	case 'x':
		theta[1] += 5;
		if (theta[1] > 360) theta[1] -= 360;
		glutPostRedisplay();
		break;
	case 'c':
		glutIdleFunc(spinArm);
		break;
	case 'v':
		theta[3] += 5;
		if (theta[3] > 150) theta[3] -= 150;
		glutPostRedisplay();
		break;
	case 'b':
		theta[5] += 5;
		if (theta[5] > 150) theta[5] -= 150;
		glutPostRedisplay();
		break;
	case 'n':
		theta[5] += 5;
		if (theta[5] > 150) theta[5] -= 150;
		glutPostRedisplay();
		break;
	case 'q':
		glutIdleFunc(sit);
		break;
	case 'N':
		glutIdleFunc(unsit);
		break;
	case 'm':
		theta[7] += 5;
		if (theta[7] > 60) theta[7] -= 60;
		glutPostRedisplay();
		break;
	case 'M':
		theta[9] += 5;
		if (theta[9] > 60) theta[9] -= 60;
		glutPostRedisplay();
		break;
		/*--------------------TỦ--------------*/
	case '1':
		Theta[2] += 5;
		glutPostRedisplay();
		break;
		/*--------TÊN LỬA------------*/
	// Quay đế
	case 'e':
		alpha += 10;
		glutPostRedisplay();
		break;
	case 'r':
		alpha -= 10;
		glutPostRedisplay();
		break;
		//nâng giá tên lửa
	case 't':
		beta += 10;
		if (beta >= 60) beta = 60;
		glutPostRedisplay();
		break;
	case 'T':
		beta -= 10;
		if (beta <= 0) beta = 0;
		glutPostRedisplay();
		break;
		//Phóng tên
	case 'y':
		yy += cao_than;
		gama -= 5;
		if (gama * -1 > beta + 10) {
			yy = 0;
			gama = 0;
		}
		glutPostRedisplay();
		break;
	case '9':
		isRorate = !isRorate;
		glutPostRedisplay();
		break;

	case '4':
		dong_ho_x += 0.1;
		glutPostRedisplay();
		break;
	case '5':
		dong_ho_x -= 0.1;
		glutPostRedisplay();
		break;
	case '6':
		dong_ho_y += 0.1;
		glutPostRedisplay();
		break;
	case '7':
		dong_ho_y -= 0.1;
		glutPostRedisplay();
		break;


	case '/':
		cam_x += 0.5;
		glutPostRedisplay();
		break;
	case '*':
		cam_x -= 0.5;
		glutPostRedisplay();
		break;
	case '-':
		cam_y += 0.5;
		glutPostRedisplay();
		break;
	case '+':
		cam_y -= 0.5;
		glutPostRedisplay();
		break;
	default:
		break;
	}

	update_camera();

}
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(640, 640);
	glutInitWindowPosition(100, 150);
	glutCreateWindow("A Cube is rotated by keyboard and shaded");
	glewInit();
	generateGeometry();
	initGPUBuffers();
	shaderSetup();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutMainLoop();
	return 0;
}
