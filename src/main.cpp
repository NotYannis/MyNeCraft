//Includes application
#include <conio.h>
#include <vector>
#include <string>
#include <windows.h>
#include <ctime>

#include "external/gl/glew.h"
#include "external/gl/freeglut.h"

//Moteur
#include "engine/utils/types_3d.h"
#include "engine/timer.h"
#include "engine/log/log_console.h"
#include "engine/render/renderer.h"
#include "engine/gui/screen.h"
#include "engine/gui/screen_manager.h"

#include "world.h"
#include "avatar.h"


NYRenderer * g_renderer = NULL;
NYTimer * g_timer = NULL;
int g_nb_frames = 0;
float g_elapsed_fps = 0;
int g_main_window_id;
int g_mouse_btn_gui_state = 0;
bool g_fullscreen = false;

//GUI 
GUIScreenManager * g_screen_manager = NULL;
GUIBouton * BtnParams = NULL;
GUIBouton * BtnClose = NULL;
GUILabel * LabelFps = NULL;
GUILabel * LabelCam = NULL;
GUIScreen * g_screen_params = NULL;
GUIScreen * g_screen_jeu = NULL;
GUISlider * g_slider;

//Timer for day passed
time_t g_startTime;
time_t g_currentTime;
float g_timeElasped;

float currentHour = 0.f;
float timeSpeed = 0.000f;
float startDay = 0.f;
float startNight = 12.0f;

NYVert3Df sunPos;
float sunAngle = 0.0f;

NYColor skyColor = NYColor(44.f / 255.f, 225.f / 255.f, 219.f / 255.f, 1);

//Variable globale
NYWorld * g_world;
NYAvatar * g_avatar;
GLuint g_program;

//////////////////////////////////////////////////////////////////////////
// GESTION APPLICATION
//////////////////////////////////////////////////////////////////////////
void update(void)
{
	float elapsed = g_timer->getElapsedSeconds(true);

	static float g_eval_elapsed = 0;

	//Calcul des fps
	g_elapsed_fps += elapsed;
	g_nb_frames++;
	if(g_elapsed_fps > 1.0)
	{
		LabelFps->Text = std::string("FPS : ") + toString(g_nb_frames);
		g_elapsed_fps -= 1.0f;
		g_nb_frames = 0;
	}

	LabelCam->Text = std::string("Cam X : ") + toString(g_renderer->_Camera->_Position.X) + 
						std::string(", Y : ") + toString(g_renderer->_Camera->_Position.Y) + 
						std::string(", Z : ") + toString(g_renderer->_Camera->_Position.Z);

	g_avatar->update(elapsed);

	//Rendu
	g_renderer->render(elapsed);

}


void render2d(void)
{
	g_screen_manager->render();
}

void renderObjects(void)
{
	
	//Rendu des axes
	/*glDisable(GL_LIGHTING);

	glBegin(GL_LINES);
	glColor3d(1,0,0);
	glVertex3d(0,0,0);
	glVertex3d(10000,0,0);
	glColor3d(0,1,0);
	glVertex3d(0,0,0);
	glVertex3d(0,10000,0);
	glColor3d(0,0,1);
	glVertex3d(0,0,0);
	glVertex3d(0,0,10000);
	glEnd();

	glBegin(GL_LINES);
	glColor3d(1, 0, 0);
	//glVertex3d(g_renderer->_Camera->_Position.X - g_renderer->_Camera->_Direction.X - 0.01, g_renderer->_Camera->_Position.Y - g_renderer->_Camera->_Direction.Y, g_renderer->_Camera->_Position.Z - g_renderer->_Camera->_Direction.Z);
	//glVertex3d(g_renderer->_Camera->_Direction.X * 2, g_renderer->_Camera->_Direction.Y * 2, g_renderer->_Camera->_Direction.Z * 2);
	glEnd();*/

	//On desactive le back face culling
	glDisable(GL_CULL_FACE);
	//On active l'illumination 
	glEnable(GL_LIGHTING);


	/*glBegin(GL_QUADS);

	//Speculaire
	GLfloat whiteSpecularMaterial[] = { 0.3, 0.3, 0.3, 1.0 };
	GLfloat mShininess = 100;

	//Emissive
	GLfloat rEmissive[] = { 0.3, 0.0, 0.0, 1.0 };
	GLfloat gEmissive[] = { 0.0, 0.3, 0.0, 1.0 };
	GLfloat bEmissive[] = { 0.0, 0.0, 0.3, 1.0 };


	//Diffuse
	GLfloat rMaterialDiffuse[] = { 0.7, 0, 0, 1.0 };
	GLfloat gMaterialDiffuse[] = { 0, 0.7, 0, 1.0 };
	GLfloat bMaterialDiffuse[] = { 0, 0, 0.7, 1.0 };


	//Ambient
	GLfloat rMaterialAmbient[] = { 0.2, 0, 0, 1.0 };
	GLfloat gMaterialAmbient[] = { 0, 0.2, 0, 1.0 };
	GLfloat bMaterialAmbient[] = { 0, 0, 0.2, 1.0 };

	//Face 6
	glMaterialfv(GL_FRONT, GL_DIFFUSE, rMaterialDiffuse);
	glMaterialfv(GL_FRONT, GL_AMBIENT, rMaterialAmbient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, whiteSpecularMaterial);
	glMaterialf(GL_FRONT, GL_SHININESS, mShininess);
	glMaterialfv(GL_FRONT, GL_EMISSION, rEmissive);

	//On set la normale
	glNormal3f(-1, 0, 0);
	glColor3d(0, 0, 1);
	glVertex3d(-1, -1, -1);
	glVertex3d(-1, -1, 1);
	glVertex3d(-1, 1, 1);
	glVertex3d(-1, 1, -1);

	glEnd();
	*/
	
	//Render the woooorld
	glUseProgram(g_program);


	//Pixel shader variables
	GLuint amb = glGetUniformLocation(g_program, "ambientLevel");
	glUniform1f(amb, 0.4);

	GLuint radius = glGetUniformLocation(g_program, "radius");
	glUniform1f(radius, 2);

	//Vertex shader variables
	GLuint elap = glGetUniformLocation(g_program, "elapsed");
	glUniform1f(elap, NYRenderer::_DeltaTimeCumul);


	GLuint invView = glGetUniformLocation(g_program, "invertView");
	glUniformMatrix4fv(invView, 1, true, g_renderer->_Camera->_InvertViewMatrix.Mat.t);

	NYFloatMatrix viewMat = g_renderer->_Camera->_InvertViewMatrix;
	viewMat.invert();
	GLuint view = glGetUniformLocation(g_program, "view");
	glUniformMatrix4fv(view, 1, true, viewMat.Mat.t);

	glPushMatrix();
	g_world->render_world_vbo();
	glPopMatrix();

	sunPos = NYVert3Df(100, 50, 1000);
	//sunPos.rotate(NYVert3Df(0, 1, 0), sunAngle);

	glPushMatrix();
	glTranslatef(sunPos.X, sunPos.Y, sunPos.Z);

	glutSetColor(0, 255, 255, 0);
	glutSolidCube(20);

	sunAngle += timeSpeed;
	if (sunAngle >= M_PI * 2) {
		sunAngle = 0.0f;
	}
	glPopMatrix();



	//Changement de la couleur de fond

	//Time manager
	currentHour = (sunAngle / (M_PI * 2)) * 24;
	if (currentHour <= startNight) {
		skyColor = skyColor.interpolate(NYColor(44.f / 255.f, 225.f / 255.f, 219.f / 255.f, 1), 0.1f);
	}
	if(currentHour > startNight){
		skyColor = skyColor.interpolate(NYColor(7.f / 255.f, 2.f / 255.f, 65.f / 255.f, 1), 0.1f);
	}
	g_renderer->setBackgroundColor(skyColor);
}



void setLights(void)
{
	//On active la light 0
	glEnable(GL_LIGHT0);


	glEnable(GL_LIGHT1);
	float position[4] = { sunPos.X, sunPos.Y, sunPos.Z, 0 }; // w = 1 donc c'est une point light (w=0 -> directionelle, point � l'infini)
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	float diffuse[4] = { 0.5f,0.5f,0.5f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

	float specular[4] = { 0.5f,0.5f,0.5f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	float ambient[4] = { 0.3f,0.3f,0.3f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	float position2[4] = { -sunPos.X, -sunPos.Y, -sunPos.Z, 0 };
	glLightfv(GL_LIGHT1, GL_POSITION, position2);

	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);

	glLightfv(GL_LIGHT1, GL_SPECULAR, specular);

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
}

void resizeFunction(int width, int height)
{
	glViewport(0, 0, width, height);
	g_renderer->resize(width,height);
}

//////////////////////////////////////////////////////////////////////////
// GESTION CLAVIER SOURIS
//////////////////////////////////////////////////////////////////////////

void specialDownFunction(int key, int p1, int p2)
{
	//On change de mode de camera
	if(key == GLUT_KEY_LEFT)
	{
	}

}

void specialUpFunction(int key, int p1, int p2)
{

}

void keyboardDownFunction(unsigned char key, int p1, int p2)
{
	if(key == VK_ESCAPE)
	{
		glutDestroyWindow(g_main_window_id);	
		exit(0);
	}

	if(key == 'f')
	{
		if(!g_fullscreen){
			glutFullScreen();
			g_fullscreen = true;
		} else if(g_fullscreen){
			glutLeaveGameMode();
			glutLeaveFullScreen();
			glutReshapeWindow(g_renderer->_ScreenWidth, g_renderer->_ScreenWidth);
			glutPositionWindow(0,0);
			g_fullscreen = false;
		}
	}

	if (key == 'z') {
		g_avatar->avance = true;
	}
	if (key == 'd') {
		g_avatar->droite = true;
	}
	if (key == 'q') {
		g_avatar->gauche = true;
	}
	if (key == 's') {
		g_avatar->recule = true;
	}

	if (key == VK_SPACE) {
		g_avatar->Jump = true;
	}
	if (key == 'a') {
		g_avatar->Crouch = true;
	}
	if (key == VK_LSHIFT) {
		g_avatar->Run = true;
	}
}

void keyboardUpFunction(unsigned char key, int p1, int p2)
{
	if (key == 'z') {
		g_avatar->avance = false;
	}
	if (key == 'd') {
		g_avatar->droite = false;
	}
	if (key == 'q') {
		g_avatar->gauche = false;
	}
	if (key == 's') {
		g_avatar->recule = false;
	}
	if (key == 'a') {
		g_avatar->Crouch = false;
	}
	if (key == VK_LSHIFT) {
		g_avatar->Run = false;
	}
}

void mouseWheelFunction(int wheel, int dir, int x, int y)
{
	g_renderer->_Camera->move(NYVert3Df(0, 0, dir));
}

void mouseFunction(int button, int state, int x, int y)
{
	//Gestion de la roulette de la souris
	if((button & 0x07) == 3 && state)
		mouseWheelFunction(button,1,x,y);
	if((button & 0x07) == 4 && state)
		mouseWheelFunction(button,-1,x,y);

	//GUI
	g_mouse_btn_gui_state = 0;
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		g_mouse_btn_gui_state |= GUI_MLBUTTON;
	
	bool mouseTraite = false;
	mouseTraite = g_screen_manager->mouseCallback(x,y,g_mouse_btn_gui_state,0,0);
}

void mouseMoveFunction(int x, int y, bool pressed)
{


	bool mouseTraite = false;

	mouseTraite = g_screen_manager->mouseCallback(x,y,g_mouse_btn_gui_state,0,0);
	
	if(pressed && mouseTraite)
	{
		//Mise a jour des variables li�es aux sliders
	}

	static int lastx = -1;
	static int lasty = -1;


	int dx = x - lastx;
	int dy = y - lasty;
	//g_renderer->_ScreenWidth, g_renderer->_ScreenHeight
	if (x < 100 || x > g_renderer->_ScreenWidth - 100 ||
		y < 100 || y > g_renderer->_ScreenHeight - 100) {
		glutWarpPointer(g_renderer->_ScreenWidth / 2, g_renderer->_ScreenHeight / 2);
		lastx = g_renderer->_ScreenWidth / 2;
		lasty = g_renderer->_ScreenHeight / 2;
	}
	else {
		lastx = x;
		lasty = y;
	}

	if (GetKeyState(VK_LCONTROL) & 0x80)
	{
		NYVert3Df strafe = g_renderer->_Camera->_NormVec;
		strafe.Z = 0;
		strafe.normalize();
		strafe *= (float)-dx / 50.0f;

		NYVert3Df avance = g_renderer->_Camera->_Direction;
		avance.Z = 0;
		avance.normalize();
		avance *= (float)dy / 50.0f;

		g_renderer->_Camera->move(avance + strafe);
	}
	else
	{
		g_renderer->_Camera->rotate((float)-dx / 300.0f);
		g_renderer->_Camera->rotateUp((float)-dy / 300.0f);
	}

	if (pressed) {
	}


}

void mouseMoveActiveFunction(int x, int y)
{
	mouseMoveFunction(x,y,true);
}

void mouseMovePassiveFunction(int x, int y)
{
	mouseMoveFunction(x,y,false);
}


void clickBtnParams (GUIBouton * bouton)
{
	g_screen_manager->setActiveScreen(g_screen_params);
}

void clickBtnCloseParam (GUIBouton * bouton)
{
	g_screen_manager->setActiveScreen(g_screen_jeu);
}


/**
  * POINT D'ENTREE PRINCIPAL
  **/
int main(int argc, char* argv[])
{ 
	LogConsole::createInstance();

	int screen_width = 800;
	int screen_height = 600;

	glutInit(&argc, argv); 
	glutInitContextVersion(3,0);
	glutSetOption(
		GLUT_ACTION_ON_WINDOW_CLOSE,
		GLUT_ACTION_GLUTMAINLOOP_RETURNS
		);

	glutInitWindowSize(screen_width,screen_height);
	glutInitWindowPosition (0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE );

	glEnable(GL_MULTISAMPLE);

	Log::log(Log::ENGINE_INFO, (toString(argc) + " arguments en ligne de commande.").c_str());	
	bool gameMode = false;
	for(int i=0;i<argc;i++)
	{
		if(argv[i][0] == 'w')
		{
			Log::log(Log::ENGINE_INFO,"Arg w mode fenetre.\n");
			gameMode = false;
		}
	}

	if(gameMode)
	{
		int width = glutGet(GLUT_SCREEN_WIDTH);
		int height = glutGet(GLUT_SCREEN_HEIGHT);
		
		char gameModeStr[200];
		sprintf(gameModeStr,"%dx%d:32@60",width,height);
		glutGameModeString(gameModeStr);
		g_main_window_id = glutEnterGameMode();
	}
	else
	{
		g_main_window_id = glutCreateWindow("MyNecraft");
		glutReshapeWindow(screen_width,screen_height);
	}

	if(g_main_window_id < 1) 
	{
		Log::log(Log::ENGINE_ERROR,"Erreur creation de la fenetre.");
		exit(EXIT_FAILURE);
	}
	
	GLenum glewInitResult = glewInit();

	if (glewInitResult != GLEW_OK)
	{
		Log::log(Log::ENGINE_ERROR,("Erreur init glew " + std::string((char*)glewGetErrorString(glewInitResult))).c_str());
		_cprintf("ERROR : %s",glewGetErrorString(glewInitResult));
		exit(EXIT_FAILURE);
	}

	//Affichage des capacit�s du syst�me
	Log::log(Log::ENGINE_INFO,("OpenGL Version : " + std::string((char*)glGetString(GL_VERSION))).c_str());

	glutDisplayFunc(update);
	glutReshapeFunc(resizeFunction);
	glutKeyboardFunc(keyboardDownFunction);
	glutKeyboardUpFunc(keyboardUpFunction);
	glutSpecialFunc(specialDownFunction);
	glutSpecialUpFunc(specialUpFunction);
	glutMouseFunc(mouseFunction);
	glutMotionFunc(mouseMoveActiveFunction);
	glutPassiveMotionFunc(mouseMovePassiveFunction);
	glutIgnoreKeyRepeat(1);

	//Initialisation du renderer
	g_renderer = NYRenderer::getInstance();
	g_renderer->setRenderObjectFun(renderObjects);
	g_renderer->setRender2DFun(render2d);
	g_renderer->setLightsFun(setLights);
	g_renderer->setBackgroundColor(NYColor());
	g_renderer->initialise(true);

	//Creation d'un programme de shader, avec vertex et fragment shaders
	g_program = g_renderer->createProgram("../src/shaders/psbase.glsl", "../src/shaders/vsbase.glsl");

	//On applique la config du renderer
	glViewport(0, 0, g_renderer->_ScreenWidth, g_renderer->_ScreenHeight);
	g_renderer->resize(g_renderer->_ScreenWidth,g_renderer->_ScreenHeight);
	
	//Ecran de jeu
	uint16 x = 10;
	uint16 y = 10;
	g_screen_jeu = new GUIScreen(); 

	g_screen_manager = new GUIScreenManager();
		
	//Bouton pour afficher les params
	BtnParams = new GUIBouton();
	BtnParams->Titre = std::string("Params");
	BtnParams->X = x;
	BtnParams->setOnClick(clickBtnParams);
	g_screen_jeu->addElement(BtnParams);

	y += BtnParams->Height + 1;

	LabelFps = new GUILabel();
	LabelFps->Text = "FPS";
	LabelFps->X = x;
	LabelFps->Y = y;
	LabelFps->Visible = true;
	g_screen_jeu->addElement(LabelFps);

	y += BtnParams->Height + 1;

	LabelCam = new GUILabel();
	LabelCam->Text = "CamPos";
	LabelCam->X = x;
	LabelCam->Y = y;
	LabelCam->Visible = true;
	g_screen_jeu->addElement(LabelCam);

	//Ecran de parametrage
	x = 10;
	y = 10;
	g_screen_params = new GUIScreen();

	GUIBouton * btnClose = new GUIBouton();
	btnClose->Titre = std::string("Close");
	btnClose->X = x;
	btnClose->setOnClick(clickBtnCloseParam);
	g_screen_params->addElement(btnClose);

	y += btnClose->Height + 1;
	y+=10;
	x+=10;

	GUILabel * label = new GUILabel();
	label->X = x;
	label->Y = y;
	label->Text = "Param :";
	g_screen_params->addElement(label);

	y += label->Height + 1;

	g_slider = new GUISlider();
	g_slider->setPos(x,y);
	g_slider->setMaxMin(1,0);
	g_slider->Visible = true;
	g_screen_params->addElement(g_slider);

	y += g_slider->Height + 1;
	y+=10;

	//Ecran a rendre
	g_screen_manager->setActiveScreen(g_screen_jeu);
	
	//Init Camera
	g_renderer->_Camera->setPosition(NYVert3Df(1000,1000,1000));
	g_renderer->_Camera->setLookAt(NYVert3Df(0,0,0));
	


	//Init World
	g_world = new NYWorld();
	g_world->_FacteurGeneration = 5;
	g_world->init_world();

	g_avatar = new NYAvatar(g_renderer->_Camera, g_world);
	//Fin init moteur

	//Init application

	glutSetCursor(GLUT_CURSOR_NONE);


	//Init Timer
	g_timer = new NYTimer();
	
	time(&g_startTime);

	//On start
	g_timer->start();

	glutMainLoop(); 

	return 0;
}

