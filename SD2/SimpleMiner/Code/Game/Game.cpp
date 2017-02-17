#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/World.hpp"
#include "Game/Chunk.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Math/LineSegment3.hpp"
#include "Engine/Math/IntVector3.hpp"
#include "Engine/File/FileUtils.hpp"
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <sstream>
/*#include <gl/GL.h>*/

//TODO: center the loaded chunks on the player and not the camera

const float END_MOVIE_FRAME_SECONDS = 0.05f;

SpriteSheet* g_tileAtlas=nullptr;
World* g_theWorld=nullptr;
BitmapFont* g_theFont=nullptr;
DebugMessagePrinter* g_theDebugMessagePrinter=nullptr;

void Game::togglePause()
	{
	if(m_isPaused)
		m_isPaused = false;
	else
		m_isPaused = true;
	}

bool Game::isQuitting()
	{
	return m_isQuittingGame;
	}

void Game::addDebugDotAt(const Vector3& pos)
	{
	m_debugDotPoints.push_back(pos);
	}

void Game::doUnitTests()
	{
	BlockInfo bi;
	bi.m_chunk=nullptr;
	IntVector3 pos=IntVector3(CHUNK_BLOCKS_X-1,CHUNK_BLOCKS_Y-1,CHUNK_BLOCKS_Z-1);
	bi.m_blockIndex=Chunk::getBlockIndexForLocalCoords(pos);

	pos=Chunk::getLocalCoordsForBlockIndex(bi.m_blockIndex);
	bi.moveNorth();
	pos=Chunk::getLocalCoordsForBlockIndex(bi.m_blockIndex);
	bi.moveSouth();
	pos=Chunk::getLocalCoordsForBlockIndex(bi.m_blockIndex);
	while(true)
		break;
	return;

	}

void Game::updateVisualInput(float deltaSeconds)
	{
	if(g_theInput==nullptr)
		return;
	if(g_theInput->wasKeyJustPressed(KEYCODE_F4))
		g_doHorizonEffect=!g_doHorizonEffect;

	if(g_theDebugMessagePrinter)
		{
		if (g_doHorizonEffect)
			g_theDebugMessagePrinter->addMessage(std::string("Applying horizon effect."),Rgba::LIME);
		else
			g_theDebugMessagePrinter->addMessage(std::string("Not applying horizon effect."),Rgba::RED);
		}
	}

void Game::deactivateAllChunks()
	{
	m_world->deactivateAllChunks();
	}

void Game::drawSelectedFaceBox() const
	{
	Chunk* chunk=m_currentRaycast.m_impactChunk;
	if(chunk==nullptr) return;

	Rect3 drawRect;

	if(m_currentRaycast.m_impactFaceNormal==Vector3::ZERO)
		return;

// 	Vector3 startpos=m_currentRaycast.m_impactPos;
// 	Vector3 endpos=startpos+m_currentRaycast.m_impactFaceNormal*BLOCK_CUBE_SIZE*2.5f;
// 	g_theRenderer->drawLine3(LineSegment3(startpos,endpos),Rgba::WHITE,10.f);

	const float SCALE=0.45f;
	const float DISPLACEMENT_FACTOR=0.55f;

	Vector3 i=Vector3::ZERO;
	Vector3 j=Vector3::ZERO;
	if (m_currentRaycast.m_impactFaceNormal==-Vector3::X_AXIS_VECTOR||m_currentRaycast.m_impactFaceNormal==Vector3::X_AXIS_VECTOR)
		{
		i=Vector3(Vector3::Y_AXIS_VECTOR*(SCALE*BLOCK_CUBE_SIZE));
		j=Vector3(Vector3::Z_AXIS_VECTOR*(SCALE*BLOCK_CUBE_SIZE));
		}
	else if (m_currentRaycast.m_impactFaceNormal==-Vector3::Y_AXIS_VECTOR||m_currentRaycast.m_impactFaceNormal==Vector3::Y_AXIS_VECTOR)
		{
		i=Vector3(Vector3::Z_AXIS_VECTOR*(SCALE*BLOCK_CUBE_SIZE));
		j=Vector3(Vector3::X_AXIS_VECTOR*(SCALE*BLOCK_CUBE_SIZE));
		}
	else if (m_currentRaycast.m_impactFaceNormal==-Vector3::Z_AXIS_VECTOR||m_currentRaycast.m_impactFaceNormal==Vector3::Z_AXIS_VECTOR)
		{
		i=Vector3(Vector3::X_AXIS_VECTOR*(SCALE*BLOCK_CUBE_SIZE));
		j=Vector3(Vector3::Y_AXIS_VECTOR*(SCALE*BLOCK_CUBE_SIZE));
		}

	drawRect.points[0]=i+j;
	drawRect.points[1]=i-j;
	drawRect.points[2]=-i-j;
	drawRect.points[3]=-i+j;
	
	drawRect+=m_currentRaycast.m_impactFaceNormal*DISPLACEMENT_FACTOR;

	Vector3 blockCenter=chunk->calcBlockCenter(m_currentRaycast.m_impactLocalCoords);
	drawRect+=blockCenter;

	g_theRenderer->enableDepthTesting(true);
	
	std::vector<Vertex3> verts;
	std::vector<Vertex3> backverts;
	Vertex3 vert;
	Vertex3 backvert;
	vert.m_color=Rgba::WHITE;
	vert.m_texcoords=Vector2(0.f,0.f);
	backvert.m_color=Rgba(0xffffff7f);
	backvert.m_texcoords=Vector2(0.f,0.f);

	for (int ipoint=0;ipoint<4;ipoint++)
		{
		vert.m_pos=backvert.m_pos=drawRect.points[ipoint];
		verts.push_back(vert);
		backverts.push_back(backvert);
		}

	g_theRenderer->drawLineLoop(verts,4.f);

	g_theRenderer->enableDepthTesting(false);

	g_theRenderer->drawLineLoop(backverts,4.f);
	}

void Game::drawViewFrustum() const
	{
	goIntoSceneRenderMode();
/*	g_theRenderer->disable(GL_TEXTURE_2D);*/
	Vector3 offset=m_cam.getForward()*0.f;
	Vector3 vfp[5];
	for(int i=0;i<5;i++)
		vfp[i]=m_cam.m_viewFrustumPoints[i]+offset;
	for (int i=1;i<5;i++)
		g_theRenderer->drawLine3(LineSegment3(vfp[0],vfp[i]),Rgba::WHITE,5.f);
	std::vector<Vertex3> boxPoints;
	for (int i=1;i<5;i++)
		boxPoints.push_back(Vertex3(vfp[i],Rgba::WHITE,Vector2(0.f,0.f)));
	g_theRenderer->drawLineLoop(boxPoints,5.f);
	}

void Game::displayHUD() const
	{
	goIntoTextRenderMode();

	displayHUDBackground();
	displayHUDBlocks();
	displayHUDShortcutKeys();

	goIntoSceneRenderMode();
	}

void Game::displayHUDBackground() const
	{
	float bottom=g_theWorldBounds->m_mins.y;
	float top=bottom+0.18f*(g_theWorldBounds->m_maxs.y-bottom);
	float left=g_theWorldBounds->m_mins.x;
	float right=g_theWorldBounds->m_maxs.x;

	float width=right-left;
	float partialWidth=width*0.45f;
	float middleX=0.5f*(right+left);
	float xofs=partialWidth*0.5f;
	left=middleX-xofs;
	right=left+partialWidth;

	Rgba darkColor=Rgba::BLACK;
	Rgba lightColor=Rgba::WHITE;
	Rgba midColor=Rgba::BLACK;
	Rgba color=Rgba(0x7f7f9fff);
	darkColor.setAlphaAsFloat(0.5f);
	lightColor.setAlphaAsFloat(0.5f);
	midColor.setAlphaAsFloat(0.25f);
	color.setAlphaAsFloat(0.65f);

	float borderThickness=4.f;

	g_theRenderer->drawFilledBox(AABB2(Vector2(left,top),Vector2(right,bottom)),color);
	g_theRenderer->drawLine(LineSegment2(Vector2(left,bottom),Vector2(right,bottom)),darkColor,borderThickness);
	g_theRenderer->drawLine(LineSegment2(Vector2(left,bottom),Vector2(left,top)),midColor,borderThickness);
	g_theRenderer->drawLine(LineSegment2(Vector2(right,bottom),Vector2(right,top)),midColor,borderThickness);
	g_theRenderer->drawLine(LineSegment2(Vector2(left,top),Vector2(right,top)),lightColor,borderThickness);
	}

void Game::displayHUDBlocks() const
	{
	//TODO: if shift is pressed, show the blocks 10-19.
// 	g_theRenderer->enable(GL_TEXTURE_2D);
	//TODO: draw isometric blocks representing all the block types available
	for(int i=BLOCK::GRASS;i<minOf((int)BLOCK::AMT_TYPES,(int)20);i++)
		{
		drawSingleHUDBlock(i);
		drawSelectedHUDBlockOutline();
		}
	}

void Game::drawSingleHUDBlock(int blockIndex) const
	{
	g_theRenderer->bindTexture(g_tileAtlas->getTexture());
	if(blockIndex>BLOCK::AMT_TYPES)
		return;
//	float left=g_theWorldBounds->m_mins.x;
	
	float boxWidth=ISOMETRIC_BLOCK_WIDTH;
	float paddedBoxWidth=boxWidth*ISOMETRIC_BLOCK_WIDTH_PADDING_FACTOR;
	float boxHalfWidth=boxWidth*0.5f;
	float boxTopHeight=boxHalfWidth*0.5f;
	float boxHeight=20.f;
	float width=paddedBoxWidth*(float)minOf((int)BLOCK::AMT_TYPES,10);
	int column=blockIndex%10;
	int row=blockIndex/10;
	float x = g_theWorldBounds->CalcCenter().x - 0.5f*width + paddedBoxWidth*((float)column + 0.5f);
	float rowsize=0.06f*g_theWorldBounds->calcHeight();
	float y = rowsize*(2.f-(float)row);
	Rgba tint=Rgba::WHITE;

	std::vector<Vertex3> verts;
	BlockDefinition bdef=BlockDefinition::s_definitions[blockIndex];
	AABB2 texCoords=bdef.m_topTexCoords;

	float z=0.f;
	
	verts.push_back(Vertex3(Vector3(x             ,y+boxTopHeight,z),tint,Vector2(texCoords.m_mins.x,texCoords.m_mins.y)));
	verts.push_back(Vertex3(Vector3(x-boxHalfWidth,y             ,z),tint,Vector2(texCoords.m_mins.x,texCoords.m_maxs.y)));
	verts.push_back(Vertex3(Vector3(x             ,y-boxTopHeight,z),tint,Vector2(texCoords.m_maxs.x,texCoords.m_maxs.y)));
	verts.push_back(Vertex3(Vector3(x+boxHalfWidth,y             ,z),tint,Vector2(texCoords.m_maxs.x,texCoords.m_mins.y)));

	texCoords=bdef.m_sideTexCoords;
	tint=Rgba::WHITE;
	tint.scaleRGB(0.75f);

	verts.push_back(Vertex3(Vector3(x-boxHalfWidth,y                       ,z),tint,Vector2(texCoords.m_mins.x,texCoords.m_mins.y)));
	verts.push_back(Vertex3(Vector3(x-boxHalfWidth,y-boxHeight             ,z),tint,Vector2(texCoords.m_mins.x,texCoords.m_maxs.y)));
	verts.push_back(Vertex3(Vector3(x             ,y-boxHeight-boxTopHeight,z),tint,Vector2(texCoords.m_maxs.x,texCoords.m_maxs.y)));
	verts.push_back(Vertex3(Vector3(x             ,y-boxTopHeight          ,z),tint,Vector2(texCoords.m_maxs.x,texCoords.m_mins.y)));

//	texCoords=bdef.m_sideTexCoords;
	tint=Rgba::WHITE;
	tint.scaleRGB(0.37f);

	verts.push_back(Vertex3(Vector3(x             ,y-boxTopHeight          ,z),tint,Vector2(texCoords.m_maxs.x,texCoords.m_mins.y)));
	verts.push_back(Vertex3(Vector3(x             ,y-boxHeight-boxTopHeight,z),tint,Vector2(texCoords.m_maxs.x,texCoords.m_maxs.y)));
	verts.push_back(Vertex3(Vector3(x+boxHalfWidth,y-boxHeight             ,z),tint,Vector2(texCoords.m_mins.x,texCoords.m_maxs.y)));
	verts.push_back(Vertex3(Vector3(x+boxHalfWidth,y                       ,z),tint,Vector2(texCoords.m_mins.x,texCoords.m_mins.y)));

	g_theRenderer->drawQuads3D(verts);
	//TODO: finish drawing box

	}

void Game::drawSelectedHUDBlockOutline() const
	{
// 	g_theRenderer->disable(GL_TEXTURE_2D);
	int blockIndex=m_selectedPlacementBlock;
	if (blockIndex>BLOCK::AMT_TYPES)
		return;
	//	float left=g_theWorldBounds->m_mins.x;

	float boxWidth=ISOMETRIC_BLOCK_WIDTH;
	float paddedBoxWidth=boxWidth*ISOMETRIC_BLOCK_WIDTH_PADDING_FACTOR;
	float boxHalfWidth=boxWidth*0.5f;
	float boxTopHeight=boxHalfWidth*0.5f;
	float boxHeight=20.f;
	float width=paddedBoxWidth*(float)minOf((int)BLOCK::AMT_TYPES,10);
	float rowsize=0.06f*g_theWorldBounds->calcHeight();
	int column=blockIndex%10;
	int row=blockIndex/10;
	float x = g_theWorldBounds->CalcCenter().x - 0.5f*width + paddedBoxWidth*((float)column + 0.5f);
	float y = rowsize*(2.f-(float)row);
	AABB2 bounds;
	bounds.m_mins=Vector2(x-0.5f*paddedBoxWidth,y-boxTopHeight-boxHeight);
	bounds.m_maxs=Vector2(x+0.5f*paddedBoxWidth,y+boxTopHeight);
	Rgba outlineColor=Rgba::WHITE;
	g_theRenderer->drawBox(bounds,outlineColor);

	Rgba tint=Rgba::WHITE;
	}

void Game::displayHUDShortcutKeys() const
	{
	//TODO: draw number shortcut keys mapped to each block type
// 	g_theRenderer->disable(GL_TEXTURE_2D);

	for (int i=BLOCK::GRASS;i<minOf((int)BLOCK::AMT_TYPES,(int)10);i++)
		{
		drawSingleBlockSelectorShortcut(i);
		}
	}

void Game::drawSingleBlockSelectorShortcut(int blockIndex) const
	{
	if(g_theFont==nullptr)
		return;
// 	glDisable(GL_TEXTURE_2D);

	if (blockIndex>BLOCK::AMT_TYPES)
		return;
	//	float left=g_theWorldBounds->m_mins.x;

	float boxWidth=ISOMETRIC_BLOCK_WIDTH;
	float paddedBoxWidth=boxWidth*ISOMETRIC_BLOCK_WIDTH_PADDING_FACTOR;
	float boxHalfWidth=boxWidth*0.5f;
	float boxTopHeight=boxHalfWidth*0.5f;
/*	float boxHeight=20.f;*/
	float width=paddedBoxWidth*(float)minOf((int)BLOCK::AMT_TYPES,10);
	float x = g_theWorldBounds->CalcCenter().x - 0.5f*width + paddedBoxWidth*((float)blockIndex + 0.5f);

	float fontHeight=15.f;
	float aspect=1.f;
	Rgba textColor=Rgba::BLUE;
	float y = 0.13f*g_theWorldBounds->calcHeight() + boxTopHeight + fontHeight;

	std::stringstream ss;
	ss.str("");
	ss<<(blockIndex);

	g_theRenderer->drawTextLineCentered(g_theFont,ss.str(),Vector2(x,y),fontHeight,aspect,textColor);

	Rgba tint=Rgba::WHITE;
	}

void Game::startScreenShake()
	{
	m_currentShakeAmount = SCREEN_SHAKE_AMOUNT;
	m_currentShakeTimeLeft = SCREEN_SHAKE_TIME_SPAN;
	}

void Game::doScreenShake() const
	{
	float screenShakeX = GetRandomFloatInRange(-m_currentShakeAmount, m_currentShakeAmount);
	float screenShakeY = GetRandomFloatInRange(-m_currentShakeAmount, m_currentShakeAmount);
	g_theRenderer->translate2D(Vector2(screenShakeX, screenShakeY));
	}

void Game::goIntoSceneRenderMode() const
	{

	g_theRenderer->setPerspectiveProjection(m_cam.m_fovVerticalDegrees,
											m_cam.m_aspectRatio,m_cam.m_nearDistance,
											m_cam.m_farDistance);

	g_theRenderer->rotate3D(-90.f,Vector3::X_AXIS_VECTOR);
	g_theRenderer->rotate3D(90.f,Vector3::Z_AXIS_VECTOR);

	g_theRenderer->rotate3D(-m_cam.m_rollDegreesAboutX,Vector3::X_AXIS_VECTOR);
	g_theRenderer->rotate3D(-m_cam.m_pitchDegreesAboutY,Vector3::Y_AXIS_VECTOR);
	g_theRenderer->rotate3D(-m_cam.m_yawDegreesAboutZ,Vector3::Z_AXIS_VECTOR);

	g_theRenderer->translate3D(-m_cam.m_pos);

	g_theRenderer->enableBackFaceCulling(true);
	g_theRenderer->enableDepthTesting(true);
	}

void Game::goIntoTextRenderMode() const
	{
	g_theRenderer->setOrthoProjection(*g_theWorldBounds);

	g_theRenderer->enableBackFaceCulling(false);
	g_theRenderer->enableDepthTesting(false);
	}

void Game::updateInput(float deltaSeconds)
	{
	updateRotationInput(deltaSeconds);
	updatePositionInput(deltaSeconds);
	updateEditingInput(deltaSeconds);
	updateBlockSelectionInput(deltaSeconds);
	updateCameraModeInput();
	updateMovementModeInput();
	updateVisualInput(deltaSeconds);
	}

void Game::updateRotationInput(float deltaSeconds)
	{
	float ROTATION_SPEED = 45.f;
	float rotAmt=ROTATION_SPEED*deltaSeconds;
	switch (m_cam.m_mode)
		{
		case CAMERA::NO_CLIP:
			if (g_theInput->isKeyDown('I'))
				m_cam.m_pitchDegreesAboutY+=rotAmt;
			if (g_theInput->isKeyDown('K'))
				m_cam.m_pitchDegreesAboutY-=rotAmt;
			if (g_theInput->isKeyDown('J'))
				m_cam.m_yawDegreesAboutZ+=rotAmt;
			if (g_theInput->isKeyDown('L'))
				m_cam.m_yawDegreesAboutZ-=rotAmt;
			if (g_theInput->isKeyDown('U'))
				m_cam.m_rollDegreesAboutX+=rotAmt;
			if (g_theInput->isKeyDown('O'))
				m_cam.m_rollDegreesAboutX-=rotAmt;
			updateCameraFromMouse();
			m_cam.m_pitchDegreesAboutY=clampFloat(m_cam.m_pitchDegreesAboutY,-89.99999f,89.999999f);
			m_cam.m_yawDegreesAboutZ=fmod(m_cam.m_yawDegreesAboutZ,360.f);
			m_cam.m_rollDegreesAboutX=fmod(m_cam.m_rollDegreesAboutX,360.f);
			break;
		case CAMERA::FIRST_PERSON:	case CAMERA::FROM_BEHIND:	case CAMERA::FIXED_ANGLE:
			Player* player=m_world->m_thePlayer;
			if (g_theInput->isKeyDown('I'))
				player->m_headPitchDegrees+=rotAmt;
			if (g_theInput->isKeyDown('K'))
				player->m_headPitchDegrees-=rotAmt;
			if (g_theInput->isKeyDown('J'))
				player->m_yawDegrees+=rotAmt;
			if (g_theInput->isKeyDown('L'))
				player->m_yawDegrees-=rotAmt;
			updatePlayerFromMouse();
			player->m_headPitchDegrees=clampFloat(player->m_headPitchDegrees,-89.99999f,89.999999f);
			player->m_yawDegrees=fmod(player->m_yawDegrees,360.f);
			break;
		}
	}

void Game::updatePositionInput(float deltaSeconds)
	{

	float moveAmt=deltaSeconds*MOVE_SPEED;
	Vector3 moveDisplacement=Vector3::ZERO;
	Vector3 forward=m_cam.getForwardXY();
	Vector3 left=m_cam.getLeftXY();
	Vector3 up=m_cam.m_mode==CAMERA::NO_CLIP ? Vector3::Z_AXIS_VECTOR : Vector3::ZERO;
	if (g_theInput->isKeyDown('W'))
		moveDisplacement+=forward;
	if (g_theInput->isKeyDown('S'))
		moveDisplacement-=forward;
	if (g_theInput->isKeyDown('A'))
		moveDisplacement+=left;
	if (g_theInput->isKeyDown('D'))
		moveDisplacement-=left;
	if (g_theInput->isKeyDown('Q'))
		moveDisplacement+=up;
	if (g_theInput->isKeyDown('E'))
		moveDisplacement-=up;
	if (g_theInput->isKeyDown(KEYCODE_SHIFT) && m_cam.m_mode==CAMERA::NO_CLIP)
		moveAmt*=10.f;

	if (g_theInput->wasKeyJustPressed(KEYCODE_F8))
		m_world->jumpToNextCity();

	switch(m_cam.m_mode)
		{
		case CAMERA::NO_CLIP:
			moveDisplacement*=moveAmt;
			m_cam.m_pos+=moveDisplacement;
			if(g_theInput->wasKeyJustPressed(KEYCODE_F7))
				bringPlayerEyeToNoClipCam();
			break;
		case CAMERA::FIRST_PERSON: case CAMERA::FROM_BEHIND: case CAMERA::FIXED_ANGLE:
			Player* player=m_world->m_thePlayer;
			moveDisplacement*=MOVE_ACCELERATION_RATE;
			player->m_moveAccel=moveDisplacement;
			if (player->m_moveMode==MOVE::WALK)
				{
				if (g_theInput->wasKeyJustPressed(KEYCODE_SPACE)  && player->canJump())
					{
					player->startJump();
					}
				else
					player->m_jumpVel=Vector3::ZERO;
				}
			else if (player->m_moveMode==MOVE::FLY)
				{
				if (g_theInput->isKeyDown(KEYCODE_SPACE))
					player->m_flyAccel=Vector3(0.f,0.f,FLY_ASCENSION_ACCELERATION_RATE);
				else if(g_theInput->isKeyDown('X'))
					player->m_flyAccel=Vector3(0.f,0.f,-FLY_ASCENSION_ACCELERATION_RATE);
				else
					player->m_flyAccel=Vector3::ZERO;
				}

			//Snowman* playerSnowman=&player->m_playerSnowman;
			//playerSnowman->reCenter(playerSnowman->m_center+moveDisplacement);
			break;
		}
	}

void Game::updateCameraFromMouse()
	{
	if (!g_theInput->doesHaveFocus())
		return;
	if(m_cam.m_mode!=CAMERA::NO_CLIP)
		return;
	IntVector2 screenCenter = g_theInput->getWindowCenter();
	IntVector2 mousepos = g_theInput->getCursorPos();
	IntVector2 mousedelta = mousepos-screenCenter;
	if(mousedelta==IntVector2::ZERO)
		return;
		
	g_theInput->centerCursorPos();

	float mouseMoveX=(float)mousedelta.x;
	float mouseMoveY=(float)mousedelta.y;
	float mouseSensitivityX=0.25f;
	float mouseSensitivityY=0.25f;
	float invertYMulitiplier=-1.f;
	if (mouseMoveX!=0)
		m_cam.m_yawDegreesAboutZ-=mouseMoveX*mouseSensitivityX;
	if (mouseMoveY!=0)
		m_cam.m_pitchDegreesAboutY-=mouseMoveY*mouseSensitivityY*invertYMulitiplier;
	}

void Game::updatePlayerFromMouse()
	{
	if (!g_theInput->doesHaveFocus())
		return;
	if (m_cam.m_mode!=CAMERA::FIRST_PERSON && m_cam.m_mode!=CAMERA::FROM_BEHIND && m_cam.m_mode!=CAMERA::FIXED_ANGLE)
		return;

	IntVector2 screenCenter = g_theInput->getWindowCenter();
	IntVector2 mousepos = g_theInput->getCursorPos();
	IntVector2 mousedelta = mousepos-screenCenter;
	if (mousedelta==IntVector2::ZERO)
		return;

	g_theInput->centerCursorPos();

	float mouseMoveX=(float)mousedelta.x;
	float mouseMoveY=(float)mousedelta.y;
	float mouseSensitivityX=0.25f;
	float mouseSensitivityY=0.25f;
	float invertYMulitiplier=-1.f;

	Player* player=m_world->m_thePlayer;
	if (mouseMoveX!=0)
		player->m_yawDegrees-=mouseMoveX*mouseSensitivityX;
	if (mouseMoveY!=0)
		player->m_headPitchDegrees-=mouseMoveY*mouseSensitivityY*invertYMulitiplier;
	}

std::string Game::makeDebugLines() const
	{
	std::stringstream ss;
	ss.str("");
	//ss<<"Mouse Displacement: "<<g_theInput->calcMouseMoveDisplacement().asString()<<std::endl;
	ss<<"Camera: "<<m_cam.asString()<<std::endl;
	int amtActiveChunks=m_world->getActiveChunkCount();
	
	float pctOfMaxChunksLoaded=(float)amtActiveChunks*(1.f/(float)MAX_CHUNKS_LOADED);
	pctOfMaxChunksLoaded*=1000.f;
	pctOfMaxChunksLoaded = (float)(int)(pctOfMaxChunksLoaded>=0.f ? pctOfMaxChunksLoaded+0.5f : pctOfMaxChunksLoaded-0.5f);
	pctOfMaxChunksLoaded/=10.f;
	
	ss<<"# of activated chunks: "<<amtActiveChunks;
	ss<<"/"<<MAX_CHUNKS_LOADED<<" ("<<pctOfMaxChunksLoaded<<"%)"<<std::endl;

	ss<<"Camera mode: "<<m_cam.getModeName()<<std::endl;
	ss<<"W,S moves camera forward and backward"  <<std::endl;
	ss<<"A,D moves camera left and right"        <<std::endl;
	ss<<"Q,E raises and lowers camera elevation" <<std::endl;
	ss<<"I,K rotates pitch"                      <<std::endl;
	ss<<"J,L rotates yaw"                        <<std::endl;
	ss<<"U,O rotates roll"                       <<std::endl;
	ss<<"F5 cycles between camera modes"         <<std::endl;
	ss<<"F6 cycles between movement modes"       <<std::endl;
	ss<<"F7 drops player from no-clip camera"    <<std::endl;
	ss<<"F8 teleports player to next city"		 <<std::endl;
	ss<<"Space bar jumps or ascends flight"      <<std::endl;
	ss<<"X descends in flight mode"              <<std::endl;
	ss<<"ESC leaves the application"             ;
	std::string result= ss.str();
	return result;
	}

void Game::drawCrosshair() const
	{
	goIntoTextRenderMode();
	AABB2 bgboxBounds=*g_theWorldBounds;
	Vector2 screenCenter = bgboxBounds.CalcCenter();
	const float DISPLACEMENT_AMOUNT=20.f;
	Vector2 ydisp(0.f,DISPLACEMENT_AMOUNT);
	Vector2 xdisp(DISPLACEMENT_AMOUNT,0.f);
	const float OUTLINE_THICKNESS=2.5f;
	const float INLINE_THICKNESS=OUTLINE_THICKNESS*0.5F;
	Vector2 inlineThickDisp=Vector2(INLINE_THICKNESS,INLINE_THICKNESS);
	Vector2 outlineThickDisp=Vector2(OUTLINE_THICKNESS,OUTLINE_THICKNESS);
	g_theRenderer->drawFilledBox(AABB2(screenCenter-ydisp-outlineThickDisp,screenCenter+ydisp+outlineThickDisp),Rgba::BLACK);
	g_theRenderer->drawFilledBox(AABB2(screenCenter-xdisp-outlineThickDisp,screenCenter+xdisp+outlineThickDisp),Rgba::BLACK);
	g_theRenderer->drawFilledBox(AABB2(screenCenter-ydisp-inlineThickDisp,screenCenter+ydisp+inlineThickDisp),Rgba::WHITE);
	g_theRenderer->drawFilledBox(AABB2(screenCenter-xdisp-inlineThickDisp,screenCenter+xdisp+inlineThickDisp),Rgba::WHITE);
	goIntoSceneRenderMode();
	}

void Game::drawDebugDots() const
	{
	if (m_debugDotPoints.size()==0)
		return;
	Rgba color=Rgba::PURPLE;
	float width=15.f;
	g_theRenderer->drawDots(m_debugDotPoints, color, width);
	g_theRenderer->enableDepthTesting(false);
	//g_theRenderer->drawLine3(LineSegment3(m_debugDotPoints.at(0),m_debugDotPoints.at(m_debugDotPoints.size()-1)),Rgba::YELLOW,5.f);
	g_theRenderer->enableDepthTesting(true);
// 	g_theRenderer->drawLine3(LineSegment3(Vector3(0.f,0.f,0.f),Vector3(500.f,500.f,500.f)),Rgba::YELLOW,5.f);
	}

void Game::displayGameDebugInfo() const
	{
	if (g_theFont == nullptr)
		{
		g_theFont = new BitmapFont(g_theRenderer,"Data/Fonts/SquirrelFixedFont.png",AMT_FONT_SHEET_TILES_X,AMT_FONT_SHEET_TILES_Y);
		if (g_theFont == nullptr) // still!?
			return;
		}
	if (g_theDebugMessagePrinter==nullptr)
		{
		g_theDebugMessagePrinter=new DebugMessagePrinter(g_theRenderer,g_theFont);
		if(g_theDebugMessagePrinter==nullptr) return;
		}

	goIntoTextRenderMode();
// 	std::string debugLine=makeDebugLines();
	Vector2 textpos;
	float fontheight=15.f;
// 	float aspect=3.f/4.f;
	float pad=2.5f;
// 	bool isCentered=false;
	textpos.x=g_theWorldBounds->m_mins.x+pad;
	textpos.y=g_theWorldBounds->m_maxs.y-pad;
// 
// 	Vector2 shadowpos=textpos;
// 	float shadowDisplacementScale=2.5f;
// 	float shadowAlpha=0.5f;
// 	float bgAlpha=0.25f;
// 	Rgba textcolor=Rgba::WHITE;
// 	Rgba shadowcolor=Rgba::BLACK;
// 	Rgba bgcolor=Rgba::BLUE;
// 	shadowcolor.setAlphaAsFloat(shadowAlpha);
// 	bgcolor.setAlphaAsFloat(bgAlpha);
// 	shadowpos+=Vector2(1.f,-1.f)*shadowDisplacementScale;
	
// 	AABB2 bgboxBounds=*g_theWorldBounds;
// 	bgboxBounds.m_mins.y=bgboxBounds.m_maxs.y-14.f*fontheight;
// 	bgboxBounds.m_maxs.x=fontheight*aspect*45.f;
// 	g_theRenderer->drawFilledBox(bgboxBounds,Rgba(bgcolor));
// 	g_theRenderer->drawBox(bgboxBounds,textcolor);
// 	g_theRenderer->drawTextLines(g_theFont,debugLine,shadowpos,fontheight,aspect,shadowcolor,isCentered);
// 	g_theRenderer->drawTextLines(g_theFont,debugLine,textpos,fontheight,aspect,textcolor,isCentered);
	float masterDebugPrintAlpha=0.25f;
	g_theDebugMessagePrinter->setAllForegroundAlphasTo(masterDebugPrintAlpha);
	g_theDebugMessagePrinter->print(textpos,fontheight);

	goIntoSceneRenderMode();
	}

void Game::updateFaceSelector()
	{

// 	if(callcount%64==0)
// 		{
		m_debugDotPoints.clear();
		const float RAY_LENGTH=8.f*BLOCK_CUBE_SIZE;

		Vector3 startPos=Vector3::ZERO;
		Vector3 forward=Vector3::X_AXIS_VECTOR;
		switch (m_cam.m_mode)
			{
			case CAMERA::NO_CLIP:
				startPos=m_cam.m_pos;
				forward=m_cam.getForward();
				break;
			case CAMERA::FIRST_PERSON:	case CAMERA::FIXED_ANGLE:	case CAMERA::FROM_BEHIND:
				Player* player=m_world->m_thePlayer;
				startPos=player->calcEyePosition();
				forward=player->calcHeadForwardVector();
				break;
			}
		bool shouldTraceRay=m_cam.m_mode==CAMERA::FIXED_ANGLE;
		m_currentRaycast=m_world->raycast(startPos,forward,RAY_LENGTH,shouldTraceRay);
// 		}
// 	++callcount;
	}

void Game::updateCamera()
	{
	Player* player=m_world->m_thePlayer;
	switch (m_cam.m_mode)
		{
		case CAMERA::FIRST_PERSON:
			m_cam.m_yawDegreesAboutZ=player->m_yawDegrees;
			m_cam.m_pitchDegreesAboutY=player->m_headPitchDegrees;
			m_cam.m_rollDegreesAboutX=0.f;
			m_cam.m_pos=player->calcEyePosition();
			break;
		case CAMERA::FROM_BEHIND:
			m_cam.m_yawDegreesAboutZ=player->m_yawDegrees;
			m_cam.m_pitchDegreesAboutY=player->m_headPitchDegrees;
			m_cam.m_rollDegreesAboutX=0.f;
			m_cam.m_pos=player->calcFromBehindPosition();
			fixThirdPersonCamModes();
			break;
		case CAMERA::FIXED_ANGLE:
			m_cam.m_yawDegreesAboutZ=45.f;
			m_cam.m_pitchDegreesAboutY=20.f;
			m_cam.m_rollDegreesAboutX=0.f;
			m_cam.m_pos=player->calcEyePosition()-m_cam.getForward()*5.f;
			fixThirdPersonCamModes();
			break;
		case CAMERA::NO_CLIP:
			break;
		}
	}

void Game::fixThirdPersonCamModes()
	{
	if(m_cam.m_mode!=CAMERA::FROM_BEHIND && m_cam.m_mode!=CAMERA::FIXED_ANGLE)
		return;

	float maxDist = 8.f;
	Vector3 camFwd=m_cam.getForward();
	Vector3 camBackward=-camFwd;
	Vector3 eyePos=m_world->m_thePlayer->calcEyePosition();
	RaycastResult rayRes=m_world->raycast(eyePos,camBackward,maxDist);
	if (!rayRes.wasHit())
		return;
	m_cam.m_pos = eyePos + camBackward * maxDist * rayRes.m_impactFraction * 0.90f;
	};


void Game::updateEditingInput(float /*deltaSeconds*/)
	{
	Player* player=m_world->m_thePlayer;
	if(player==nullptr)
		return;
	while (true)
		{
		if (g_theInput->wasKeyJustPressed(KEYCODE_LMOUSEBUTTON)||g_theInput->wasKeyJustPressed(KEYCODE_RMOUSEBUTTON))
			break;
		return;
		}
	Chunk* hitchunk=m_currentRaycast.m_impactChunk;
	if(hitchunk==nullptr) return;
	Block* hitblock=hitchunk->getBlockAddrByIndex(m_currentRaycast.m_blockIndex);
	if(hitblock==nullptr) return;
	Chunk* nchunk=nullptr;
	if (g_theInput->wasKeyJustPressed(KEYCODE_RMOUSEBUTTON))
		{
		ChunkCoords chunkCoords=hitchunk->m_chunkCoords;
		IntVector3 blockDisp(m_currentRaycast.m_impactFaceNormal);
		IntVector3 blockCoords=m_currentRaycast.m_impactLocalCoords;
		ChunkCoords neighborChunkCoords;
		IntVector3 neighborBlockCoords;
		if (m_world->gotoNeighboringBlock(neighborChunkCoords,neighborBlockCoords,chunkCoords,blockCoords,blockDisp))
			{
			nchunk = m_world->getChunkAtCoords(neighborChunkCoords);
			if (nchunk)
				{
				Block* nblock=nchunk->getBlockAddrAt(neighborBlockCoords);
				if (nblock && nblock->m_type==BLOCK::AIR)
					{
					nblock->setType((unsigned char)m_selectedPlacementBlock);
					nchunk->updateLightingAfterBlockPlacementAt(neighborBlockCoords);
					nchunk->m_isVboDirty=true;
					nchunk->setToWasChanged();
					player->playBlockPlacementSound(nblock->m_type);
					//nchunk->makeNeighborsDirty();
					}
				}
			}


		//TODO: add block on a block voxel adjacent to the raycast hit block based on hit face.
		}
	else if (g_theInput->wasKeyJustPressed(KEYCODE_LMOUSEBUTTON))
		{
		player->playBlockRemovalSound(hitblock->m_type);
/*		if(hitblock->m_type!=BLOCK::WATER)*/
			hitblock->setType((unsigned char)BLOCK::AIR);
		//TODO: update lighting after block removal.
		IntVector3 blockCoords=Chunk::getLocalCoordsForBlockIndex(m_currentRaycast.m_blockIndex);
		hitchunk->updateLightingAfterBlockRemovalAt(blockCoords);
		hitchunk->m_isVboDirty=true;
		hitchunk->setToWasChanged();
		hitchunk->makeNeighborsDirty();
		}
	}

void Game::updateBlockSelectionInput(float /*deltaSeconds*/)
	{
	for (char key='0';key<(char)minOf((int)BLOCK::AMT_TYPES,(int)10)+'0';key++)
		{
		if (g_theInput->wasKeyJustPressed(key))
			{
			unsigned char blockID;
			blockID = key-'0';
			if(g_theInput->isKeyDown(KEYCODE_CONTROL))
				blockID += 10;
			//TODO: allow water selection only when I have a way to handle it.
// 			if ((BLOCK::Type)blockID==BLOCK::WATER)
// 				continue;
			m_selectedPlacementBlock=(BLOCK::Type)BlockDefinition::clampBlockIndex(blockID);
			if(m_menuSelectAudioChannel&&g_theAudioSys)
				g_theAudioSys->stopChannel(m_menuSelectAudioChannel);
			m_menuSelectAudioChannel=g_theAudioSys->playSound(m_menuSelectSound,0.9f);
			return;
			}
		}
	}

void Game::updateCameraModeInput()
	{
	if(g_theInput->wasKeyJustPressed(KEYCODE_F5))
		m_cam.selectNextMode();
	}

void Game::bringPlayerEyeToNoClipCam()
	{
	Player* player=m_world->m_thePlayer;
	Vector3 eyePos=player->calcEyePosition();
	Vector3 playerCenter=player->getPosition();
	Vector3 centerDisplacement=playerCenter-eyePos;
	Vector3 newCenter=m_cam.m_pos+centerDisplacement;
	player->m_playerSnowman.reCenter(newCenter);
	}

void Game::updateMovementModeInput()
	{
	if(g_theInput->wasKeyJustPressed(KEYCODE_F6))
		m_world->m_thePlayer->cycleMovementMode();
	}

void Game::initSettings()
	{
	SettingsFileData sfd;
	std::vector<unsigned char> buffer;
	if (!readBufferFromFile(buffer,"Data/SaveSlot0/GameSettings.bin") || buffer.size()<sizeof(sfd))
		{
		sfd.m_version=CURRENT_GAME_VERSION;
		sfd.m_camera.m_mode=CAMERA::FIRST_PERSON;
		sfd.m_camera.m_pos = Vector3((float)CHUNK_BLOCKS_X*0.5f,(float)CHUNK_BLOCKS_Y*0.5f,(float)CHUNK_BLOCKS_Z);
		sfd.m_camera.m_rollDegreesAboutX = 0.f;
		sfd.m_camera.m_pitchDegreesAboutY = 0.f;
		sfd.m_camera.m_yawDegreesAboutZ   = 0.f;
		sfd.m_seed=(unsigned int)time(nullptr);
		}
	else
		{
		int amtBytes=sizeof(sfd);
		unsigned char* fileDataAsArray=(unsigned char*)&sfd;
		for (int i=0;i<amtBytes;i++)
			{
			fileDataAsArray[i]=buffer[i];
			}
		}

	m_cam=sfd.m_camera;
	m_world->m_thePlayer->setPosition(m_cam.m_pos);
	m_world->m_seed=sfd.m_seed;
	}

void Game::saveSettings()
	{
	SettingsFileData sfd;
	std::vector<unsigned char> buffer;

	int amtBytes=sizeof(sfd);
	unsigned char* fileDataAsArray=(unsigned char*)&sfd;

	sfd.m_camera=m_cam;
	sfd.m_seed=m_world->m_seed;
	sfd.m_version=CURRENT_GAME_VERSION;

	buffer.resize(sizeof(sfd));
	for (int i=0;i<amtBytes;i++)
		{
		buffer[i]=fileDataAsArray[i];
		}

	writeBufferToFile(buffer,"Data/SaveSlot0/GameSettings.bin");
	}

Game::Game()
	: m_timescale(10)
	, m_isPaused(false)
	, m_isQuittingGame(false)
	, m_currentShakeAmount(0.f)
	, m_currentShakeTimeLeft(0.f)
	, m_selectedPlacementBlock(BLOCK::GRAY_BRICK)
	, m_cam()
	, m_menuSelectSound(0)
	, m_menuSelectAudioChannel((AudioChannelHandle)nullptr)
	{
	init();
	}

void Game::init()
	{
	//g_tileAtlas=m_tileAtlas=g_theRenderer->createOrGetSpriteSheet(std::string("Data/Images/Test_StbiAndDirectX.png"),16,16);
	g_tileAtlas=m_tileAtlas=g_theRenderer->createOrGetSpriteSheet(std::string("Data/Images/SimpleMinerAtlas.png"),16,16);
	BlockDefinition::initBlockDefinitions();
	BlockDefinition::initBlockWalkSounds();
	BlockDefinition::initBlockPlacementSounds();
	BlockDefinition::initBlockRemovalSounds();

	m_world=g_theWorld=new World(&m_cam.m_pos);

	initSettings();

	g_doShowDebugGraphics = true;

	doUnitTests();

	m_cam.m_fovVerticalDegrees=60.f;
	m_cam.m_aspectRatio=g_worldWidth/g_worldHeight;
	m_cam.m_nearDistance=0.01f;
	m_cam.m_farDistance=1000.f;

	if (g_theAudioSys)
		m_menuSelectSound=g_theAudioSys->createOrGetSound(std::string("Data/Audio/menuSelect.ogg"));
	}

Game::~Game()
	{
	delete m_world;
	m_world=nullptr;
	}

void Game::update(float deltaSeconds)
	{
	if(g_theDebugMessagePrinter)
		g_theDebugMessagePrinter->clear();
	if(g_theInput->isKeyDown('T'))
		{
		deltaSeconds*=0.1f;
		if(g_theDebugMessagePrinter)
			g_theDebugMessagePrinter->addMessage(std::string("Slow-Motion Mode(10% time scale)"),Rgba::LIME);
		}

	//float curtime=(float)GetCurrentTimeSeconds();

	if(m_world)
		m_world->update(deltaSeconds);

	updateScreenShakeTimer(deltaSeconds);

	updateFaceSelector();

	updateInput(deltaSeconds);

	if (g_theDebugMessagePrinter)
		g_theDebugMessagePrinter->addMessage(makeDebugLines(),Rgba::WHITE);

	updateCamera();
	}

void Game::updateScreenShakeTimer(float deltaSeconds)
	{
	if (m_currentShakeTimeLeft > 0.f)
		m_currentShakeTimeLeft -= (m_currentShakeTimeLeft > deltaSeconds ? deltaSeconds : m_currentShakeTimeLeft);
	if (m_currentShakeTimeLeft == 0.f)
		m_currentShakeAmount = 0.f;
	}

void Game::render() const
	{
	g_theRenderer->clearScreenAndDepth(Rgba::SKYBLUE);

	goIntoSceneRenderMode();

	if(m_tileAtlas==nullptr)
		return;

	Texture* voxelTexture=m_tileAtlas->getTexture();
	if (voxelTexture)
		g_theRenderer->bindTexture(voxelTexture);

	if(m_world)
		m_world->render();

	drawSelectedFaceBox();

	g_theRenderer->drawAxes(1000.f);

	drawViewFrustum();

	if (g_doShowDebugGraphics)
		{
		drawDebugDots();
		displayGameDebugInfo();
		}

	drawCrosshair();

	displayHUD();
	}


void Game::toggleDebugGraphics()
	{
	g_doShowDebugGraphics = !g_doShowDebugGraphics;
	}