// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <SFML/Audio.hpp>
#include <SSVStart.h>
#include "Data/StyleData.h"
#include "Global/Assets.h"
#include "Global/Config.h"
#include "Utils/Utils.h"
#include "MenuGame.h"

using namespace std;
using namespace sf;
using namespace ssvs;
using namespace ssvs::Utils;
using namespace sses;

namespace hg
{
	MenuGame::MenuGame(HexagonGame& mHexagonGame, GameWindow& mGameWindow) : hexagonGame(mHexagonGame), window(mGameWindow)
	{
		// Title bar
		getAssetManager().getTexture("titleBar.png").setSmooth(true);
		titleBar.setOrigin({0, 0});
		titleBar.setScale({0.5f, 0.5f});
		titleBar.setPosition(overlayCamera.getConvertedCoords({20, 20}));

		// Credits bar 1
		getAssetManager().getTexture("creditsBar1.png").setSmooth(true);
		creditsBar1.setOrigin({1024, 0});
		creditsBar1.setScale({0.373f, 0.373f});
		creditsBar1.setPosition(overlayCamera.getConvertedCoords(Vector2i(getWidth() - 20.f, 20.f)));
		
		// Credits bar 2
		getAssetManager().getTexture("creditsBar2.png").setSmooth(true);
		creditsBar2.setOrigin({1024, 116});
		creditsBar2.setScale({0.373f, 0.373f});
		creditsBar2.setPosition(overlayCamera.getConvertedCoords(Vector2i(getWidth() - 20.f, 160.f / getZoomFactor())));

		// Bottom bar
		getAssetManager().getTexture("bottomBar.png").setSmooth(true);
		float scaleFactor{getWidth() * getZoomFactor() / 2048.f};
		bottomBar.setOrigin({0, 112.f});
		bottomBar.setScale({scaleFactor, scaleFactor});
		bottomBar.setPosition(overlayCamera.getConvertedCoords(Vector2i(0, getHeight())));

		game.onUpdate += [&](float mFrameTime) { update(mFrameTime); };
		game.onDraw += [&]{ draw(); };

		levelDataIds = getMenuLevelDataIdsByPack(getPackPaths()[packIndex]);
		setIndex(0);
		
		if(getProfilesSize() == 0) state = StateType::PROFILE_CREATION;
		else if(getProfilesSize() == 1)
		{
			setCurrentProfile(getFirstProfileName());
			state = StateType::LEVEL_SELECTION;
		}
	}

	void MenuGame::init()
	{
		stopAllMusic();
		stopAllSounds();

		playSound("openHexagon.ogg");
	}

	void MenuGame::setIndex(int mIndex)
	{
		currentIndex = mIndex;

		if(currentIndex > (int)(levelDataIds.size() - 1)) currentIndex = 0;
		else if(currentIndex < 0) currentIndex = levelDataIds.size() - 1;

		levelData = getLevelData(levelDataIds[currentIndex]);
		styleData = getStyleData(levelData.getStyleId());
		difficultyMultipliers = levelData.getDifficultyMultipliers();
		difficultyMultIndex = find(begin(difficultyMultipliers), end(difficultyMultipliers), 1) - begin(difficultyMultipliers);
	}

	void MenuGame::update(float mFrameTime)
	{
		if(inputDelay <= 0)
		{
			if(window.isKeyPressed(Keyboard::LAlt) && window.isKeyPressed(Keyboard::Return))
			{
				setFullscreen(window, !window.getFullscreen());
				inputDelay = 25;
			}
			else if(window.isKeyPressed(Keyboard::Escape)) inputDelay = 25;
		}
		else
		{
			inputDelay -= 1 * mFrameTime;
			if(inputDelay < 1.0f && window.isKeyPressed(Keyboard::Escape)) window.stop();
		}

		if(state == StateType::PROFILE_CREATION)
		{
			Event e;
			window.pollEvent(e);

			if(e.type == Event::TextEntered)
			{
				if (e.text.unicode > 47 && e.text.unicode < 126) if (profileCreationName.size() < 16) profileCreationName.append(toStr((char)(e.text.unicode)));
				if (e.text.unicode == 8) if(!profileCreationName.empty()) profileCreationName.erase(profileCreationName.end() - 1);
				if (e.text.unicode == 13) if(!profileCreationName.empty())
				{
					createProfile(profileCreationName);
					setCurrentProfile(profileCreationName);
					inputDelay = 30;
					state = StateType::LEVEL_SELECTION;
				}
			}
		}
		else if(state == StateType::PROFILE_SELECTION)
		{
			vector<string> profileNames{getProfileNames()};
			profileCreationName = profileNames[profileIndex % profileNames.size()];

			if(inputDelay <= 0)
			{
				if(window.isKeyPressed(Keyboard::Left))
				{
					playSound("beep.ogg");
					profileIndex--;
					
					inputDelay = 14;
				}
				else if(window.isKeyPressed(Keyboard::Right))
				{
					playSound("beep.ogg");
					profileIndex++;

					inputDelay = 14;
				}
				else if(window.isKeyPressed(Keyboard::Return))
				{
					playSound("beep.ogg");
					setCurrentProfile(profileCreationName);
					state = StateType::LEVEL_SELECTION;

					inputDelay = 30;
				}
				else if(window.isKeyPressed(Keyboard::F1))
				{
					playSound("beep.ogg");
					profileCreationName = "";
					state = StateType::PROFILE_CREATION;

					inputDelay = 14;
				}
			}
		}
		else if (state == StateType::LEVEL_SELECTION)
		{
			styleData.update(mFrameTime);

			backgroundCamera.rotate(levelData.getRotationSpeed() * 10 * mFrameTime);

			if(inputDelay <= 0)
			{
				if(window.isKeyPressed(Keyboard::Right))
				{
					playSound("beep.ogg");
					setIndex(currentIndex + 1);

					inputDelay = 14;
				}
				else if(window.isKeyPressed(Keyboard::Left))
				{
					playSound("beep.ogg");
					setIndex(currentIndex - 1);

					inputDelay = 14;
				}
				else if(window.isKeyPressed(Keyboard::Up))
				{
					playSound("beep.ogg");
					difficultyMultIndex++;
					inputDelay = 12;
				}
				else if(window.isKeyPressed(Keyboard::Down))
				{
					playSound("beep.ogg");
					difficultyMultIndex--;
					inputDelay = 12;
				}
				else if(window.isKeyPressed(Keyboard::Return))
				{
					playSound("beep.ogg");
					window.setGame(&hexagonGame.getGame());
					hexagonGame.newGame(levelDataIds[currentIndex], true, difficultyMultipliers[difficultyMultIndex % difficultyMultipliers.size()]);

					inputDelay = 14;
				}
				else if(window.isKeyPressed(Keyboard::F2) || window.isKeyPressed(Keyboard::J))
				{
					playSound("beep.ogg");
					profileCreationName = "";
					state = StateType::PROFILE_SELECTION;

					inputDelay = 14;
				}
				else if(window.isKeyPressed(Keyboard::F3) || window.isKeyPressed(Keyboard::K))
				{
					playSound("beep.ogg");
					setPulse(!getPulse());

					inputDelay = 14;
				}
				else if (window.isKeyPressed(Keyboard::F4) || window.isKeyPressed(Keyboard::L))
				{
					playSound("beep.ogg");

					packIndex = (packIndex + 1) % getPackPaths().size();
					levelDataIds = getMenuLevelDataIdsByPack(getPackPaths()[packIndex]);
					setIndex(0);

					inputDelay = 14;
				}
			}
		}
	}
	void MenuGame::draw()
	{
		window.clear(Color{0, 0, 0, 0});
		
		if(state == StateType::LEVEL_SELECTION)
		{
			window.clear(styleData.getColors()[0]);
			backgroundCamera.apply();
			styleData.drawBackground(window.getRenderWindow(), Vector2f{0,0}, 6);

			overlayCamera.apply();
			drawLevelSelection();
			render(bottomBar);
		}
		else if(state == StateType::PROFILE_CREATION)
		{
			window.clear(Color::Black);

			overlayCamera.apply();
			drawProfileCreation();
		}
		else if(state == StateType::PROFILE_SELECTION)
		{
			window.clear(Color::Black);

			overlayCamera.apply();
			drawProfileSelection();
		}

		overlayCamera.apply();
		render(titleBar);
		render(creditsBar1);
		render(creditsBar2); 
	}

	void MenuGame::positionAndDrawCenteredText(Text& mText, Color mColor, float mElevation, bool mBold)
	{
		//mText.setOrigin(mText.getGlobalBounds().width / 2, 0);
		if(mBold) mText.setStyle(Text::Bold);
		mText.setColor(mColor);
		mText.setPosition(overlayCamera.getConvertedCoords({20, 0}).x, titleBar.getGlobalBounds().top + titleBar.getGlobalBounds().height + mElevation);
		render(mText);
	}

	void MenuGame::drawLevelSelection()
	{		
		Color mainColor{styleData.getMainColor()};
		MusicData musicData{getMusicData(levelData.getMusicId())};
		PackData packData{getPackData(levelData.getPackPath().substr(6, levelData.getPackPath().size() - 7))};
		string packName{packData.getName()};


		cProfText.setString("profile: " + getCurrentProfile().getName());
		positionAndDrawCenteredText(cProfText, mainColor, 0, false);

		cProfText.setString("pulse: " + (getPulse() ? toStr("enabled") : toStr("disabled")));
		positionAndDrawCenteredText(cProfText, mainColor, 20, false);

		cProfText.setString("pack: " + packName + " (" + toStr(packIndex + 1) + "/" + toStr(getPackPaths().size()) + ")");
		positionAndDrawCenteredText(cProfText, mainColor, 40, false);

		cProfText.setString("best time: " + toStr(getScore(getScoreValidator(levelData.getId(), difficultyMultipliers[difficultyMultIndex % difficultyMultipliers.size()]))));
		positionAndDrawCenteredText(cProfText, mainColor, 60, false);

		if(difficultyMultipliers.size() > 1)
		{
			cProfText.setString("difficulty: " + toStr(difficultyMultipliers[difficultyMultIndex % difficultyMultipliers.size()]));
			positionAndDrawCenteredText(cProfText, mainColor, 80, false);
		}

		levelName.setString(levelData.getName());
		positionAndDrawCenteredText(levelName, mainColor, 50 + 120, false);

		levelDesc.setString(levelData.getDescription());
		positionAndDrawCenteredText(levelDesc, mainColor, 50 + 195 + 60*(countNewLines(levelData.getName())), false);

		levelAuth.setString("author: " + levelData.getAuthor());
		positionAndDrawCenteredText(levelAuth, mainColor, -30 + 500, false);
		
		levelMusc.setString("music: " + musicData.getName() + " by " + musicData.getAuthor() + " (" + musicData.getAlbum() + ")");
		positionAndDrawCenteredText(levelMusc, mainColor, -30 + 515, false);

		levelMusc.setString("(" + toStr(currentIndex + 1) + "/" + toStr(levelDataIds.size()) + ")");
		positionAndDrawCenteredText(levelMusc, mainColor, -30 + 530, false);
	}
	void MenuGame::drawProfileCreation()
	{
		Color mainColor{Color::White};

		cProfText.setString("profile creation");
		positionAndDrawCenteredText(cProfText, mainColor, 768 - 395, false);

		cProfText.setString("insert profile name");
		positionAndDrawCenteredText(cProfText, mainColor, 768 - 375, false);

		cProfText.setString("press enter when done");
		positionAndDrawCenteredText(cProfText, mainColor, 768 - 335, false);

		cProfText.setString("keep esc pressed to exit");
		positionAndDrawCenteredText(cProfText, mainColor, 768 - 315, false);

		levelName.setString(profileCreationName);
		positionAndDrawCenteredText(levelName, mainColor, 768 - 245 - 40, false);
	}
	void MenuGame::drawProfileSelection()
	{
		Color mainColor{Color::White};

		cProfText.setString("profile selection");
		positionAndDrawCenteredText(cProfText, mainColor, 768 - 395, false);

		cProfText.setString("press left/right to browse profiles");
		positionAndDrawCenteredText(cProfText, mainColor, 768 - 375, false);

		cProfText.setString("press enter to select profile");
		positionAndDrawCenteredText(cProfText, mainColor, 768 - 355, false);

		cProfText.setString("press f1 to create a new profile");
		positionAndDrawCenteredText(cProfText, mainColor, 768 - 335, false);

		levelName.setString(profileCreationName);
		positionAndDrawCenteredText(levelName, mainColor, 768 - 245 - 40, false);
	}

	void MenuGame::render(Drawable &mDrawable) { window.draw(mDrawable); }

	GameState& MenuGame::getGame() { return game; }
}
