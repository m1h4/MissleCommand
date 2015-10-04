#include "nGlobals.h"
#include "nMain.h"
#include "nFile.h"
#include "nGame.h"

nGame::nGame(void) :
m_PauseTime(NULL),
m_Pause(true)	// Default
{
}

nGame::~nGame(void)
{
}

bool nGame::InitGame()
{
	// Reset score and health
	SetScore(0);
	SetHealth(100);

	// Reset missle time
	SetMissleTime(GAME_MISSLE_TIME);

	// Add two silos
	AddSilo(nVector2(nGetInstance()->GetGraphics()->GetDisplaySize().cx/3.0f,nGetInstance()->GetGraphics()->GetDisplaySize().cy),GAME_FRENDLY_COLOR);
	AddSilo(nVector2(nGetInstance()->GetGraphics()->GetDisplaySize().cx/1.3f,nGetInstance()->GetGraphics()->GetDisplaySize().cy),GAME_FRENDLY_COLOR);

	// Reset cursor position
	nGetInstance()->GetInput()->GetMouse()->SetPosition(nVector2(nGetInstance()->GetGraphics()->GetDisplaySize().cx / 2,nGetInstance()->GetGraphics()->GetDisplaySize().cy / 2));

	// Start paused
	SetPause(true);

	return true;
}

void nGame::RestartGame()
{
	// Clean previous game objects
	CleanGame();

	// Initialize new game objects
	InitGame();
}

void nGame::CleanGame()
{
	// Clear the silos array
	ClearSilos();

	// Clear the missles array
	ClearMissles();

	// Clear the explosions array
	ClearExplosions();

	// Create the particle array
	ClearParticles();
}

void nGame::Render()
{
	// Draw silos
	for(unsigned long i = 0; i < m_Silos.size(); i++)
		RenderSilo(m_Silos[i]);

	// Draw missle data
	for(unsigned long i = 0; i < m_Missles.size(); i++)
		RenderMissle(m_Missles[i]);

	// Draw explosion data
	for(unsigned long i = 0; i < m_Explosions.size(); i++)
		RenderExplosion(m_Explosions[i]);

	// Draw particle data
	for(unsigned long i = 0; i < m_Particles.size(); i++)
		RenderParticle(m_Particles[i]);

	// Draw the cursor
	if(nGetInstance()->GetInput()->GetMouse()->GetButton(nMouse::ButtonLeft) || nGetInstance()->GetInput()->GetMouse()->GetButton(nMouse::ButtonRight))
		nGetInstance()->GetGraphics()->DrawCursor(nGetInstance()->GetInput()->GetMouse()->GetPosition(),2,4,nGetInstance()->GetAbsoluteTime() * 10.0f,nColor(1.0f,1.0f,1.0f));
	else
		nGetInstance()->GetGraphics()->DrawCursor(nGetInstance()->GetInput()->GetMouse()->GetPosition(),2,6,0.0f,nColor(1.0f,1.0f,1.0f));

	// Draw the players score
	char buffer[512],buffer2[512];
	_snprintf(buffer,sizeof(buffer),"Score %d\nHealth %d\nLevel %d",m_Score,m_Health,(unsigned long)(30 - m_MissleTime));
	nGetInstance()->GetGraphics()->DrawText(nGetInstance()->GetGraphics()->GetBoldFont(),nRect(5,5,nGetInstance()->GetGraphics()->GetDisplaySize().cx - 5,0),buffer,DT_NOCLIP|DT_RIGHT,COLOR_TEXT);

	if(m_Pause)
	{
		unsigned long y = nGetInstance()->GetGraphics()->GetDisplaySize().cy / 3;
		nRect rect,rect2;

		_snprintf(buffer,sizeof(buffer),"Paused");

		nGetInstance()->GetGraphics()->GetTextRect(nGetInstance()->GetGraphics()->GetBoldFont(),buffer,&rect,NULL);
		nGetInstance()->GetGraphics()->DrawPrintLine(nGetInstance()->GetGraphics()->GetBoldFont(),nRect(nGetInstance()->GetGraphics()->GetDisplaySize().cx / 2 - rect.GetWidth() / 2,y,0,0),buffer,100.0f * (nGetInstance()->GetAbsoluteTime() - m_PauseTime),DT_NOCLIP,nColor(abs(sinf(nGetInstance()->GetAbsoluteTime())),COLOR_TEXT));

		y += rect.GetHeight();
		y += rect.GetHeight();

		static char time[100];
		GetTimeFormat(LOCALE_USER_DEFAULT,NULL,NULL,NULL,time,100);

		static char date[100];
		GetDateFormat(LOCALE_USER_DEFAULT,DATE_LONGDATE,NULL,NULL,date,100);

		_snprintf(buffer,sizeof(buffer),"Current time is");
		_snprintf(buffer2,sizeof(buffer2)," %s %s",time,date);

		nGetInstance()->GetGraphics()->GetTextRect(nGetInstance()->GetGraphics()->GetBoldFont(),buffer,&rect,NULL);
		nGetInstance()->GetGraphics()->GetTextRect(nGetInstance()->GetGraphics()->GetBoldFont(),buffer2,&rect2,NULL);

		nGetInstance()->GetGraphics()->DrawPrintLine(nGetInstance()->GetGraphics()->GetBoldFont(),nRect(nGetInstance()->GetGraphics()->GetDisplaySize().cx / 2 - rect.GetWidth() / 2 - rect2.GetWidth() / 2,y,0,0),buffer,100.0f * (nGetInstance()->GetAbsoluteTime() - m_PauseTime),DT_NOCLIP,nColor(0.25f,COLOR_TEXT));
		nGetInstance()->GetGraphics()->DrawPrintLine(nGetInstance()->GetGraphics()->GetBoldFont(),nRect(nGetInstance()->GetGraphics()->GetDisplaySize().cx / 2 + rect.GetWidth() / 2 - rect2.GetWidth() / 2,y,0,0),buffer2,100.0f * (nGetInstance()->GetAbsoluteTime() - m_PauseTime),DT_NOCLIP,nColor(0.5f,COLOR_TEXT));

		y += rect.GetHeight();
		y += rect.GetHeight();

		// Draw the hiscores if any
		if(m_HiScores.size())
		{
			_snprintf(buffer,sizeof(buffer),"Time");

			nGetInstance()->GetGraphics()->GetTextRect(nGetInstance()->GetGraphics()->GetBoldFont(),buffer,&rect,NULL);
			nGetInstance()->GetGraphics()->DrawPrintLine(nGetInstance()->GetGraphics()->GetBoldFont(),nRect(nGetInstance()->GetGraphics()->GetDisplaySize().cx / 2 - rect.GetWidth() - 10,y,0,0),buffer,100.0f * (nGetInstance()->GetAbsoluteTime() - m_PauseTime),DT_NOCLIP,COLOR_TEXT);

			_snprintf(buffer,sizeof(buffer),"Score");

			nGetInstance()->GetGraphics()->GetTextRect(nGetInstance()->GetGraphics()->GetBoldFont(),buffer,&rect,NULL);
			nGetInstance()->GetGraphics()->DrawPrintLine(nGetInstance()->GetGraphics()->GetBoldFont(),nRect(nGetInstance()->GetGraphics()->GetDisplaySize().cx / 2 + 10,y,0,0),buffer,100.0f * (nGetInstance()->GetAbsoluteTime() - m_PauseTime),DT_NOCLIP,COLOR_TEXT);

			y += rect.GetHeight();
			y += rect.GetHeight();

			for(unsigned long i = 0; i < m_HiScores.size(); i++)
			{
				_snprintf(buffer,sizeof(buffer),"%s",m_HiScores[i].time.c_str());

				nGetInstance()->GetGraphics()->GetTextRect(nGetInstance()->GetGraphics()->GetBoldFont(),buffer,&rect,NULL);
				nGetInstance()->GetGraphics()->DrawPrintLine(nGetInstance()->GetGraphics()->GetBoldFont(),nRect(nGetInstance()->GetGraphics()->GetDisplaySize().cx / 2 - rect.GetWidth() - 10,y,0,0),buffer,100.0f * (nGetInstance()->GetAbsoluteTime() - m_PauseTime),DT_NOCLIP,nColor(0.25f,COLOR_TEXT));
				
				_snprintf(buffer,sizeof(buffer),"%d",m_HiScores[i].score);

				nGetInstance()->GetGraphics()->GetTextRect(nGetInstance()->GetGraphics()->GetBoldFont(),buffer,&rect,NULL);
				nGetInstance()->GetGraphics()->DrawPrintLine(nGetInstance()->GetGraphics()->GetBoldFont(),nRect(nGetInstance()->GetGraphics()->GetDisplaySize().cx / 2 + 10,y,0,0),buffer,100.0f * (nGetInstance()->GetAbsoluteTime() - m_PauseTime),DT_NOCLIP,nColor(0.5f,COLOR_TEXT));

				y += rect.GetHeight();
			}

			y += rect.GetHeight();
		}

		_snprintf(buffer,sizeof(buffer),"Command");

		nGetInstance()->GetGraphics()->GetTextRect(nGetInstance()->GetGraphics()->GetBoldFont(),buffer,&rect,NULL);
		nGetInstance()->GetGraphics()->DrawPrintLine(nGetInstance()->GetGraphics()->GetBoldFont(),nRect(nGetInstance()->GetGraphics()->GetDisplaySize().cx / 2 - rect.GetWidth() - 10,y,0,0),buffer,100.0f * (nGetInstance()->GetAbsoluteTime() - m_PauseTime),DT_NOCLIP,COLOR_TEXT);

		_snprintf(buffer,sizeof(buffer),"Button");

		nGetInstance()->GetGraphics()->GetTextRect(nGetInstance()->GetGraphics()->GetBoldFont(),buffer,&rect,NULL);
		nGetInstance()->GetGraphics()->DrawPrintLine(nGetInstance()->GetGraphics()->GetBoldFont(),nRect(nGetInstance()->GetGraphics()->GetDisplaySize().cx / 2 + 10,y,0,0),buffer,100.0f * (nGetInstance()->GetAbsoluteTime() - m_PauseTime),DT_NOCLIP,COLOR_TEXT);

		y += rect.GetHeight();
		y += rect.GetHeight();

		nString keyboard = nGetInstance()->GetInput()->GetKeyboard()->GetDeviceName();
		nString mouse = nGetInstance()->GetInput()->GetMouse()->GetDeviceName();

		_snprintf(buffer,sizeof(buffer),"Pause/Unpause");

		nGetInstance()->GetGraphics()->GetTextRect(nGetInstance()->GetGraphics()->GetBoldFont(),buffer,&rect,NULL);
		nGetInstance()->GetGraphics()->DrawPrintLine(nGetInstance()->GetGraphics()->GetBoldFont(),nRect(nGetInstance()->GetGraphics()->GetDisplaySize().cx / 2 - rect.GetWidth() - 10,y,0,0),buffer,100.0f * (nGetInstance()->GetAbsoluteTime() - m_PauseTime),DT_NOCLIP,nColor(0.25f,COLOR_TEXT));
				
		_snprintf(buffer,sizeof(buffer),"%s %s or %s %s",keyboard.c_str(),nGetInstance()->GetInput()->GetKeyboard()->GetKeyName(DIK_PAUSE).c_str(),keyboard.c_str(),nGetInstance()->GetInput()->GetKeyboard()->GetKeyName(DIK_P).c_str());

		nGetInstance()->GetGraphics()->GetTextRect(nGetInstance()->GetGraphics()->GetBoldFont(),buffer,&rect,NULL);
		nGetInstance()->GetGraphics()->DrawPrintLine(nGetInstance()->GetGraphics()->GetBoldFont(),nRect(nGetInstance()->GetGraphics()->GetDisplaySize().cx / 2 + 10,y,0,0),buffer,100.0f * (nGetInstance()->GetAbsoluteTime() - m_PauseTime),DT_NOCLIP,nColor(0.5f,COLOR_TEXT));

		y += rect.GetHeight();

		_snprintf(buffer,sizeof(buffer),"Fire missle");

		nGetInstance()->GetGraphics()->GetTextRect(nGetInstance()->GetGraphics()->GetBoldFont(),buffer,&rect,NULL);
		nGetInstance()->GetGraphics()->DrawPrintLine(nGetInstance()->GetGraphics()->GetBoldFont(),nRect(nGetInstance()->GetGraphics()->GetDisplaySize().cx / 2 - rect.GetWidth() - 10,y,0,0),buffer,100.0f * (nGetInstance()->GetAbsoluteTime() - m_PauseTime),DT_NOCLIP,nColor(0.25f,COLOR_TEXT));
				
		_snprintf(buffer,sizeof(buffer),"%s %s",mouse.c_str(),nGetInstance()->GetInput()->GetMouse()->GetButtonName(nMouse::ButtonLeft).c_str());

		nGetInstance()->GetGraphics()->GetTextRect(nGetInstance()->GetGraphics()->GetBoldFont(),buffer,&rect,NULL);
		nGetInstance()->GetGraphics()->DrawPrintLine(nGetInstance()->GetGraphics()->GetBoldFont(),nRect(nGetInstance()->GetGraphics()->GetDisplaySize().cx / 2 + 10,y,0,0),buffer,100.0f * (nGetInstance()->GetAbsoluteTime() - m_PauseTime),DT_NOCLIP,nColor(0.5f,COLOR_TEXT));

		y += rect.GetHeight();

		_snprintf(buffer,sizeof(buffer),"Fire cannon");

		nGetInstance()->GetGraphics()->GetTextRect(nGetInstance()->GetGraphics()->GetBoldFont(),buffer,&rect,NULL);
		nGetInstance()->GetGraphics()->DrawPrintLine(nGetInstance()->GetGraphics()->GetBoldFont(),nRect(nGetInstance()->GetGraphics()->GetDisplaySize().cx / 2 - rect.GetWidth() - 10,y,0,0),buffer,100.0f * (nGetInstance()->GetAbsoluteTime() - m_PauseTime),DT_NOCLIP,nColor(0.25f,COLOR_TEXT));
				
		_snprintf(buffer,sizeof(buffer),"%s %s",mouse.c_str(),nGetInstance()->GetInput()->GetMouse()->GetButtonName(nMouse::ButtonRight).c_str());

		nGetInstance()->GetGraphics()->GetTextRect(nGetInstance()->GetGraphics()->GetBoldFont(),buffer,&rect,NULL);
		nGetInstance()->GetGraphics()->DrawPrintLine(nGetInstance()->GetGraphics()->GetBoldFont(),nRect(nGetInstance()->GetGraphics()->GetDisplaySize().cx / 2 + 10,y,0,0),buffer,100.0f * (nGetInstance()->GetAbsoluteTime() - m_PauseTime),DT_NOCLIP,nColor(0.5f,COLOR_TEXT));

		y += rect.GetHeight();
	}
}

void nGame::RenderSilo(const Silo& silo)
{
	// Set line width
	nGetInstance()->GetGraphics()->SetLineWidth(3);

	// Get silo view direction (it's towards the cursor)
	nVector2 dir = nGetInstance()->GetInput()->GetMouse()->GetPosition() - silo.position;
	dir.Normalize();
	dir *= 15.0f;

	// Set line coordinates
	nVector2 lines[2] = {silo.position,silo.position + dir};

	// Draw the line
	nGetInstance()->GetGraphics()->DrawLines(lines,2,nColor(0.1f,0.1f,1.0f));
}

void nGame::RenderMissle(const Missle& missle)
{
	nGetInstance()->GetGraphics()->SetLineWidth(3);

	// Fade the missle if it's dying
	float alpha = 1.0f;

	if(missle.dead > 0.0f)
		alpha = (10.0f - missle.dead) / 10.0f;

	nVector2 line[2];

	// Draw missle trail
	if(missle.trail)
	{
		line[0] = missle.source;
		line[1] = missle.position;
		nGetInstance()->GetGraphics()->DrawLines(line,2,nColor(alpha,missle.color));
	}

	// Don't draw missle head if it exploded
	if(missle.dead == 0.0f)
	{
		// Get missle direction vector for head drawing
		nVector2 dir = missle.destination - missle.source;
		dir.Normalize();

		// Draw missle head
		line[0] = missle.position;
		line[1] = missle.position + dir * 5.0f;

		nGetInstance()->GetGraphics()->DrawLines(line,2,nColor(alpha,0.1f,1.0f,0.1f));
	}

	// Draw missle destination target
	if(missle.frendly && missle.dead == 0.0f)
	{
		if(missle.trail)
			nGetInstance()->GetGraphics()->DrawTarget(nPoint(missle.destination.x,missle.destination.y),missle.target,nColor(1.0f - missle.target,GAME_FRENDLY_COLOR));
		else
			nGetInstance()->GetGraphics()->DrawTarget(nPoint(missle.destination.x,missle.destination.y),0.0f,GAME_FRENDLY_COLOR);
	}
}

void nGame::RenderExplosion(const Explosion& explosion)
{
	nGetInstance()->GetGraphics()->SetLineWidth(2);

	// Draw explosion shock-waves
	nGetInstance()->GetGraphics()->DrawCircle(nPoint(explosion.position.x,explosion.position.y),explosion.size,30,nColor(1.0f - explosion.size / explosion.power,explosion.color));
	nGetInstance()->GetGraphics()->DrawCircle(nPoint(explosion.position.x,explosion.position.y),explosion.size * explosion.size / explosion.power,30,nColor(1.0f - explosion.size * explosion.size / explosion.power / explosion.power,explosion.color));
}

void nGame::Process()
{
	static float absoluteTime = 0.0f;
	static float absoluteTime2 = 0.0f;

	// Check for pause key
	if(nGetInstance()->GetInput()->GetKeyboard()->GetNewKey(DIK_PAUSE) || nGetInstance()->GetInput()->GetKeyboard()->GetNewKey(DIK_P))
		SetPause(!GetPause());

	// Don't process further if paused
	if(m_Pause)
		return;

	absoluteTime += nGetInstance()->GetElapsedTime();
	absoluteTime2 += nGetInstance()->GetElapsedTime();

	// Create enemy missle if enough time passed
	if(absoluteTime > m_MissleTime)
	{
		// Reset the timer
		absoluteTime = 0.0f;
		
		if(m_MissleTime > 1.0f)
			m_MissleTime -= (double)nGetInstance()->GetElapsedTime() / 4.0f;

		// Create a missle at a random top position going to a random bottom position
		AddMissle(nVector2(randf(0.0f,nGetInstance()->GetGraphics()->GetDisplaySize().cx),0.0f),nVector2(randf(0.0f,nGetInstance()->GetGraphics()->GetDisplaySize().cx),nGetInstance()->GetGraphics()->GetDisplaySize().cy),1.0f,80.0f,GAME_ENEMY_COLOR,false,true);
	}

	// Create child enemy missle if enough time passed
	if(absoluteTime2 > 100.0f)
	{
		// Reset the timer
		absoluteTime2 = 0.0f;

		if(m_Missles.size())
		{
			// Create a missle at a random trail of a nother missle
			unsigned long count = 1 + rand() % 3;

			Missle* missle = &m_Missles[rand() % m_Missles.size()];

			// Find a live enemy missle
			while(missle->frendly || missle->dead > 0.0f)
				missle = &m_Missles[rand() % m_Missles.size()];

			nVector2 dir = missle->position - missle->source;
			float length = dir.Length();
			dir.Normalize();

			nVector2 source = missle->source + dir * randf(0.0f,length);

			for(unsigned long i = 0; i < count; i++)
				AddMissle(source,nVector2(randf(0.0f,nGetInstance()->GetGraphics()->GetDisplaySize().cx),nGetInstance()->GetGraphics()->GetDisplaySize().cy),1.0f,80.0f,GAME_ENEMY_COLOR,false,true);
		}
	}

	// Check left missle silo launch
	if(nGetInstance()->GetInput()->GetMouse()->GetNewButton(nMouse::ButtonLeft))
		FireSilo(m_Silos[0],nGetInstance()->GetInput()->GetMouse()->GetPosition(),20.0f,80.0f,true);
	
	// Check right missle silo launch
	if(nGetInstance()->GetInput()->GetMouse()->GetButton(nMouse::ButtonRight))
	{
		// Get a random vector in a sphere domain
		nVector2 miss(randf(-1.0f,1.0f),randf(-1.0f,1.0f));
		miss.Normalize();
		miss *= randf(0.0f,50.0f);

		FireSilo(m_Silos[1],nGetInstance()->GetInput()->GetMouse()->GetPosition() + miss,30.0f,20.0f,false);
	}

	// Remove silo fire time
	for(unsigned long i = 0; i < m_Silos.size(); i++)
	{
		Silo* silo = &m_Silos[i];

		// Remove some fire time
		silo->time -= nGetInstance()->GetElapsedTime();

		// Check if all/too much fire time was removed
		if(silo->time < 0.0f)
			silo->time = 0.0f;
	}

	// Move missles and remove dead ones
	for(unsigned long i = 0; i < m_Missles.size(); i++)
	{
		Missle* missle = &m_Missles[i];

		if(missle->position == missle->destination || missle->dead > 0.0f)
		{
			if(missle->position == missle->destination && !missle->frendly && missle->dead == 0.0f)
			{
				// Players base got hit	
				SetHealth(GetHealth() - 10);

				// Check if player dead
				if(!GetHealth())
				{
					// Game over
					AddHiScore(m_Score);
					RestartGame();

					return;
				}
			}

			if(missle->dead > 10.0f)
			{
				// Remove missle
				RemoveMissle(i--);
				continue;
			}

			if(missle->dead == 0.0f)
			{
				// Explode
				AddExplosion(missle->position,missle->blast,4.0f,nColor(1.0f,0.1f,1.0f));
			}

			missle->dead += nGetInstance()->GetElapsedTime();
			continue;
		}

		if(missle->target > 0.0f)
			missle->target -= nGetInstance()->GetElapsedTime() / 10.0f;
		else if(missle->target < 0.0f)
			missle->target = 0.0f;

		nVector2 velocity = missle->destination - missle->source;
		velocity.Normalize();

		velocity *= missle->speed * nGetInstance()->GetElapsedTime();

		if(velocity.x > 0.0f && missle->position.x + velocity.x > missle->destination.x)
			missle->position.x = missle->destination.x;
		else if(velocity.x < 0.0f && missle->position.x + velocity.x < missle->destination.x)
			missle->position.x = missle->destination.x;
		else
			missle->position.x += velocity.x;

		if(velocity.y > 0.0f && missle->position.y + velocity.y > missle->destination.y)
			missle->position.y = missle->destination.y;
		else if(velocity.y < 0.0f && missle->position.y + velocity.y < missle->destination.y)
			missle->position.y = missle->destination.y;
		else
			missle->position.y += velocity.y;
	}

	// Expand explosions and remove dead ones
	for(unsigned long i = 0; i < m_Explosions.size(); i++)
	{
		Explosion* explosion = &m_Explosions[i];

		if(explosion->size == explosion->power)
		{
			// Remove it
			RemoveExplosion(i--);
			continue;
		}

		float expansion = explosion->speed * nGetInstance()->GetElapsedTime();

		if(explosion->size + expansion > explosion->power)
			explosion->size = explosion->power;
		else
			explosion->size += expansion;

		// Check if the explosion destroyed any missles
		for(unsigned long j = 0; j < m_Missles.size(); j++)
		{
			Missle* missle = &m_Missles[j];

			// Skip frendly or already dead missles
			if(missle->frendly || missle->dead > 0.0f)
				continue;

			nVector2 dir = missle->position - explosion->position;

			unsigned long points = 0;

			if(dir.Length() < explosion->size)
			{
				// Set death time
				missle->dead += nGetInstance()->GetElapsedTime();

				// Create some particles
				for(unsigned long k = 0; k < 50; k++)
				{
					// Get a random vector in a sphere domain
					nVector2 velocity(randf(-1.0f,1.0f),randf(-1.0f,1.0f));
					velocity.Normalize();
					velocity *= randf(0.0f,5.0f);

					AddParticle(missle->position,velocity,nVector2(0.0f,0.1f),10.0f,GAME_COLOR_PARTICLES);
				}

				// Add player points
				points++;
			}

			// Add score
			if(points > 1)
				SetScore(GetScore() + points + points / 2);
			else
				SetScore(GetScore() + points);
		}
	}

	// Move and remove particles
	for(unsigned long i = 0; i < m_Particles.size(); i++)
	{
		Particle* particle = &m_Particles[i];

		// Remove the particle if it's dead
		if(particle->lived >= particle->life)
		{
			// Remove it
			RemoveParticle(i--);
			continue;
		}

		particle->lived += nGetInstance()->GetElapsedTime();

		particle->velocity += particle->force * nGetInstance()->GetElapsedTime();
		particle->position += particle->velocity * nGetInstance()->GetElapsedTime();
	}
}

bool nGame::ReadHiScores()
{
	// Set the path
	char buffer[MAX_PATH];
	PathCombine(buffer,nGetInstance()->GetModulePath().c_str(),"HiScores.sav");

	nFile file;

	// Open the file
	if(!file.Open(buffer,nFile::OpenFlagExisting|nFile::OpenFlagRead|nFile::OpenFlagShareRead))
	{
		Trace(__FUNCTION__" Failed to load hiscore file \"%s\".\n",buffer);
		return false;
	}

	// Check file type
	unsigned long magic;
	double version;

	file.Read(&magic);
	file.Read(&version);

	// Check if invalid file
	if(magic != SETTINGS_SCORE_MAGIC)
		return false;

	// Check if invalid version
	if(version != SETTINGS_SCORE_VERSION)
		return false;

	// Clear scores
	while(m_HiScores.size())
		m_HiScores.pop_back();

	// Read hiscore count
	unsigned long count;
	file.Read(&count);

	// Read hiscores
	for(unsigned long i = 0; i < count; i++)
	{
		HiScore hiscore;

		// Read the time
		file.Read(&hiscore.time);

		// Read the score
		file.Read(&hiscore.score);

		// Add the score
		m_HiScores.push_back(hiscore);
	}

	// Close the file
	file.Close();

	Trace(__FUNCTION__" Hiscores file loaded from \"%s\".\n",buffer);
	return true;
}

bool nGame::ReadHiScoresXml()
{
	char buffer[MAX_PATH];
	PathCombine(buffer,nGetInstance()->GetModulePath().c_str(),"HiScores.xml");

	// Load the config xml document
	TiXmlDocument doc(buffer);
	if(!doc.LoadFile())
	{
		Trace(__FUNCTION__" Failed to load hiscore file \"%s\".\n",buffer);
		return false;
	}

	// Get the config element
	TiXmlElement* root = doc.FirstChildElement("hiscores");
	if(!root)
	{
		Trace(__FUNCTION__" Failed to find root element \"hiscores\" in hiscores file.\n");
		return false;
	}

	// Clear scores
	while(m_HiScores.size())
		m_HiScores.pop_back();

	root = root->FirstChildElement("score");

	while(root)
	{
		HiScore score;

		if(root->Attribute("time") && root->Attribute("value"))
		{
			score.time = root->Attribute("time");
			score.score = strtoul(root->Attribute("value"),NULL,10);

			m_HiScores.push_back(score);
		}

		root = root->NextSiblingElement("score");
	}

	Trace(__FUNCTION__" Xml hiscores file loaded from \"%s\".\n",buffer);
	return true;
}

bool nGame::WriteHiScores()
{
	// Don't write anything if no scores
	if(!m_HiScores.size())
		return true;

	// Set the path
	char buffer[MAX_PATH];
	PathCombine(buffer,nGetInstance()->GetModulePath().c_str(),"HiScores.sav");

	nFile file;

	// Create/open the file
	if(!file.Open(buffer,nFile::OpenFlagCreate|nFile::OpenFlagWrite))
	{
		Trace(__FUNCTION__" Failed to create hiscore file \"%s\".\n",buffer);
		return false;
	}

	// Write the magic and vesion numbers
	file.Write((unsigned long)SETTINGS_SCORE_MAGIC);
	file.Write((double)SETTINGS_SCORE_VERSION);

	// Write the score count
	file.Write((unsigned long)m_HiScores.size());

	// Write the scores
	for(unsigned long i = 0; i < m_HiScores.size(); i++)
	{
		// Write the time
		file.Write(m_HiScores[i].time);

		// Write the score
		file.Write(m_HiScores[i].score);
	}

	// Close the file
	file.Close();

	Trace(__FUNCTION__" Hiscores file saved to \"%s\".\n",buffer);
	return true;
}

bool nGame::WriteHiScoresXml()
{
	if(!m_HiScores.size())
		return true;

	char buffer[MAX_PATH];
	PathCombine(buffer,nGetInstance()->GetModulePath().c_str(),"HiScores.xml");

	TiXmlDocument doc(buffer);

	TiXmlElement root("hiscores");

	for(unsigned long i = 0; i < m_HiScores.size(); i++)
	{
		HiScore* hiscore = &m_HiScores[i];

		TiXmlElement node("score");

		node.SetAttribute("time",hiscore->time.c_str());
		node.SetAttribute("value",hiscore->score);

		root.InsertEndChild(node);
	}

	doc.InsertEndChild(root);

	if(!doc.SaveFile())
	{
		Trace(__FUNCTION__" Failed to save hiscore file \"%s\".\n",buffer);
		return false;
	}

	Trace(__FUNCTION__" Xml hiscores file saved to \"%s\".\n",buffer);
	return true;
}

bool nGame::AddHiScore(unsigned long value)
{
	if(!value)
		return false;

	static const unsigned long maxScores = 20;

	for(unsigned long i = 0; i < m_HiScores.size(); i++)
		if(m_HiScores[i].score < value)
			break;

	HiScore hiscore;

	static char time[100];
	GetTimeFormat(LOCALE_USER_DEFAULT,NULL,NULL,NULL,time,100);

	static char date[100];
	GetDateFormat(LOCALE_USER_DEFAULT,DATE_LONGDATE,NULL,NULL,date,100);

	hiscore.score = value;
	hiscore.time = nString(time) + " " + nString(date);

	if(i < m_HiScores.size())
		m_HiScores.insert(m_HiScores.begin() + i,hiscore);
	else
		m_HiScores.push_back(hiscore);

	while(m_HiScores.size() > maxScores)
		m_HiScores.pop_back();

	return true;
}

bool nGame::RemoveHiScore(unsigned long index)
{
	if(index >= m_HiScores.size())
		return false;

	m_HiScores.erase(m_HiScores.begin() + index);

	return true;
}

void nGame::ClearHiScore()
{
	while(m_HiScores.size())
		m_HiScores.pop_back();
}

bool nGame::AddSilo(const nVector2& position,const nColor& color)
{
	Silo silo;

	silo.position = position;
	silo.time = 0.0f;

	m_Silos.push_back(silo);

	return true;
}

bool nGame::FireSilo(Silo& silo,const nVector2& target,float speed,float blast,bool trail)
{
	if(silo.time != 0.0f)
		return false;

	AddMissle(silo.position,target,speed,blast,GAME_FRENDLY_COLOR,true,trail);

	if(trail)
		silo.time = 5.0f;
	else
		silo.time = 1.0f;

	return true;
}

bool nGame::RemoveSilo(unsigned long index)
{
	if(index >= m_Silos.size())
		return false;

	m_Silos.erase(m_Silos.begin() + index);

	return true;
}

void nGame::ClearSilos()
{
	while(m_Silos.size())
		m_Silos.pop_back();
}

bool nGame::AddMissle(const nVector2& source,const nVector2& destination,float speed,float blast,const nColor& color,bool frendly,bool trail)
{
	Missle missle;

	missle.source = source;
	missle.position = missle.source;
	missle.destination = destination;
	missle.speed = speed;
	missle.blast = blast;
	missle.color = color;
	missle.frendly = frendly;
	missle.trail = trail;
	missle.dead = 0.0f;
	missle.target = 1.0f;

	m_Missles.push_back(missle);

	return true;
}

bool nGame::RemoveMissle(unsigned long index)
{
	if(index >= m_Missles.size())
		return false;

	m_Missles.erase(m_Missles.begin() + index);

	return true;
}

void nGame::ClearMissles()
{
	while(m_Missles.size())
		m_Missles.pop_back();
}

bool nGame::AddExplosion(const nVector2& position,float power,float speed,const nColor& color)
{
	Explosion explosion;

	explosion.position = position;
	explosion.power = power;
	explosion.size = 0.0f;
	explosion.speed = speed;
	explosion.color = color;

	m_Explosions.push_back(explosion);

	return true;
}

bool nGame::RemoveExplosion(unsigned long index)
{
	if(index >= m_Explosions.size())
		return false;

	m_Explosions.erase(m_Explosions.begin() + index);

	return true;
}

void nGame::ClearExplosions()
{
	while(m_Explosions.size())
		m_Explosions.pop_back();
}

bool nGame::AddParticle(const nVector2& position,const nVector2& velocity,const nVector2& force,float life,const nColor& color)
{
	Particle particle;

	particle.position = position;
	particle.velocity = velocity;
	particle.force = force;
	particle.color = color;
	particle.life = life;
	particle.lived = 0.0f;

	m_Particles.push_back(particle);

	return true;
}

void nGame::RenderParticle(const Particle& particle)
{
	nGetInstance()->GetGraphics()->SetLineWidth(3);

	nVector2 lines[2] = {particle.position,particle.position - particle.velocity};

	nGetInstance()->GetGraphics()->DrawLines(lines,2,nColor(1.0f - particle.lived / particle.life,particle.color));
}

bool nGame::RemoveParticle(unsigned long index)
{
	if(index >= m_Particles.size())
		return false;

	m_Particles.erase(m_Particles.begin() + index);

	return true;
}

void nGame::ClearParticles()
{
	while(m_Particles.size())
		m_Particles.pop_back();
}

void nGame::SetPause(bool pause)
{
	if(m_Pause == pause)
		return;

	m_Pause = pause;
	m_PauseTime = nGetInstance()->GetAbsoluteTime();
}