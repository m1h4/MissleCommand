#pragma once

#include "nColor.h"
#include "nVector.h"

#define GAME_MISSLE_TIME 30.0f
#define GAME_FRENDLY_COLOR nColor(0.1f,0.1f,1.0f)
#define GAME_ENEMY_COLOR nColor(1.0f,0.1f,0.1f)
#define GAME_COLOR_PARTICLES nColor(0.1f,1.1f,0.1f)

struct Silo
{
	nVector2		position;

	nColor			color;

	float			time;
};

struct Missle
{
	nVector2		source;
	nVector2		destination;
	nVector2		position;

	nColor			color;

	bool			frendly;

	float			blast;
	float			speed;
	float			dead;

	bool			trail;

	float			target;	// Timer for the target animation
};

struct Explosion
{
	nVector2		position;

	nColor			color;

	float			size;
	float			speed;
	float			power;
};

struct HiScore
{
	nString			time;
	unsigned long	score;
};

struct Particle
{
	nVector2		position;
	nVector2		velocity;
	nVector2		force;

	nColor			color;

	float			lived;
	float			life;
};

class nGame
{
public:
	nGame(void);
	~nGame(void);

	bool InitGame();
	void CleanGame();

	void RestartGame();

	void Render();
	void Process();

	void SetPause(bool pause);
	bool GetPause() const { return m_Pause; }

	bool AddHiScore(unsigned long value);
	bool RemoveHiScore(unsigned long index);
	void ClearHiScore();

	bool AddSilo(const nVector2& position,const nColor& color);
	bool FireSilo(Silo& silo,const nVector2& target,float speed,float blast,bool trail);
	void RenderSilo(const Silo& silo);
	bool RemoveSilo(unsigned long index);
	void ClearSilos();

	bool AddMissle(const nVector2& source,const nVector2& destination,float speed,float blast,const nColor& color,bool frendly,bool trail);
	void RenderMissle(const Missle& missle);
	bool RemoveMissle(unsigned long index);
	void ClearMissles();

	bool AddExplosion(const nVector2& position,float power,float speed,const nColor& color);
	void RenderExplosion(const Explosion& explosion);
	bool RemoveExplosion(unsigned long index);
	void ClearExplosions();

	bool AddParticle(const nVector2& position,const nVector2& velocity,const nVector2& force,float life,const nColor& color);
	void RenderParticle(const Particle& particle);
	bool RemoveParticle(unsigned long index);
	void ClearParticles();

	void RenderCursor(const nVector2& position,unsigned long internal,unsigned long external,float angle,const nColor& color);

	void SetScore(unsigned long score) { m_Score = score; }
	unsigned long GetScore() const { return m_Score; }

	void SetHealth(unsigned long health) { m_Health = health; }
	unsigned long GetHealth() const { return m_Health; }

	void SetMissleTime(double time) { m_MissleTime = time; }
	double GetMissleTime() const { return m_MissleTime; }

	bool ReadHiScores();
	bool ReadHiScoresXml();

	bool WriteHiScores();
	bool WriteHiScoresXml();

protected:
	nArray<Silo>		m_Silos;
	nArray<Missle>		m_Missles;
	nArray<Explosion>	m_Explosions;
	nArray<HiScore>		m_HiScores;
	nArray<Particle>	m_Particles;

	unsigned long		m_Score;
	unsigned long		m_Health;

	bool				m_Pause;
	double				m_PauseTime;

	double				m_MissleTime;
};
