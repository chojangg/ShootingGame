#include <iostream>
#include <cstdio>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

using namespace std;
using namespace sf;

int bullet_wait = 0;  //static 변수 실제 생성

int textPrint(Text& textMsg, Font& font, int size, float x, float y, const Color& color, string p)
{
    textMsg.setFont(font);             //폰트
    textMsg.setCharacterSize(size);    //크기 
    textMsg.setPosition(x, y);         //x, y 위치
    textMsg.setFillColor(color);       //색
    textMsg.setString(p); //출력할 문자열

    return 0;
}

int main()
{
    //윈도우 생성
    sf::ContextSettings settings;
    sf::RenderWindow window(sf::VideoMode(800, 600), "Shooting Game", sf::Style::Default, settings);
    window.setVerticalSyncEnabled(true);

    // 리소스 로드 
    //이미지 로드
    sf::Image image1;
    image1.loadFromFile("image/plane.gif");                   
    image1.createMaskFromColor(sf::Color(0, 0, 0));    //투명픽셀 색 지정

    sf::Image image2;
    image2.loadFromFile("image/bullet.png");          
    image2.createMaskFromColor(sf::Color(0, 0, 0));    //투명픽셀 색 지정

    sf::Image image3;
    image3.loadFromFile("image/ban.png");                
    image3.createMaskFromColor(sf::Color(0, 0, 0));    //투명픽셀 색 지정

    sf::Image image4;
    image4.loadFromFile("image/score.png");
    image4.createMaskFromColor(sf::Color(0, 0, 0));    //투명픽셀 색 지정

    sf::Image image5;
    image5.loadFromFile("image/background.png");
    image5.createMaskFromColor(sf::Color(0, 0, 0));    //투명픽셀 색 지정

    //텍스처 생성
    sf::Texture texture1;
    if (!texture1.loadFromImage(image1))
    {
        return 1;
    }
    sf::Texture texture2;
    if (!texture2.loadFromImage(image2))
    {
        return 1;
    }
    sf::Texture texture3;
    if (!texture3.loadFromImage(image3))
    {
        return 1;
    }
    sf::Texture texture4;
    if (!texture4.loadFromImage(image4))
    {
        return 1;
    }
    sf::Texture texture5;
    if (!texture5.loadFromImage(image5))
    {
        return 1;
    }

    //폰트 파일 로드
    Font font;
    font.loadFromFile("fonts/font.ttf");
    Text text1;
    Text text2;
    string msgStr;

    //스프라이트 생성과 텍스처 지정
    sf::Sprite background;
    background.setTexture(texture5);
    background.setPosition(0,0);

    sf::Sprite ship;
    ship.setTexture(texture1);
    ship.setPosition(300.0, 400.0);

    // score
    sf::Sprite scoreImg;
    scoreImg.setTexture(texture4);
    scoreImg.setPosition(0, 10);
    int game_score = 0;
    String game_score2;


    sf::Sprite bullet[20];     //총알 20개
    int bullet_live[20];     //활성 여부 1(활성) 0(비활성)
    int bcounter = 0;        //발사될 총알의 인덱스
    for (int i = 0; i < 20; i++) {
        bullet[i].setTexture(texture2);
        bullet_live[i] = 0;
    }

    sf::Sprite target;
    target.setTexture(texture3);
    float target_velx = 5;       //수평방향 속도 +, - 로 변동
    target.scale(1.0, 1.0);      //출력시 이미지 확대 비율

    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("sound/exp.wav"))
        return 1;

    sf::Sound sound(buffer);
    
    // 메인루프
    int n = 0;
    while (window.isOpen())
    {
        //--------------------- 입력 이벤트 처리 ----------------------
        // 1) 모든 윈도우 이벤트를 체크한다.
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) { //닫기 버튼 이벤트
                window.close();
            }
            if (event.type == sf::Event::KeyPressed) {  //키누름 이벤트
                if (event.key.code == sf::Keyboard::Escape) {  //Esc 키
                    window.close();
                }
            }
        }

        // 2)키보드 상태 실시간 검사 (이벤트 검사 대신 직접 키누름 상태 검사)
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) { //상하좌우 키 누름에 따라 우주선 이동
            ship.move(-5, 0);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            ship.move(5, 0);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            ship.move(0, -5);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            ship.move(0, 5);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) { //space 키 누름시 총알 발사
            //적당한 시간 간격으로 bcounter 인덱스의 bullet 발사하기
            if (bullet_wait == 0) {     //기다릴 필요시간 0이면 bullet 발사
                printf("\n--Fire Bullet--\n");
                bullet[bcounter].setPosition(ship.getPosition());    //bullet 위치를 ship 위치로 지정
                bullet[bcounter].move(100, -80);                     // 위치 보정해준다.
                bullet_live[bcounter] = 1;                        //발사 상태
                bcounter++;        //인덱스 증가
                if (bcounter >= 20)
                    bcounter = 0;        //배열 범위 넘지 않도록해주기

                bullet_wait = 5;              //다음발사 기다릴 시간 --> 5 으로
            }
        }

        // 자동 움직임 부분
        // bullet 들 움직이기
        for (int i = 0; i < 20; i++) {
            sf::Vector2f bpos = bullet[i].getPosition();
            if (bullet_live[i] == 1 && bpos.y >= -100)
                bullet[i].move(0, -15);
            if (bpos.y < -100)      //화면 벗어나면 비활성 상태로
                bullet_live[i] = 0;

        }
        // 발사 기다릴 시간 감소시키기
        if (bullet_wait > 0)
            bullet_wait--;

        // 타겟 이동시키기(좌우 이동, 애니메이션)
        target.move(target_velx, 0);
        sf::Vector2f targetpos = target.getPosition();
        if (targetpos.x > 800 + 50 || targetpos.x < -50)   //화면을 벗어나면 속도를 반대로 한다.
            target_velx = -target_velx;


        //충돌체크
        sf::Vector2f targetpos2 = target.getPosition();
        for (int i = 0; i < 20; i++) {
            sf::Vector2f bpos2 = bullet[i].getPosition();
            float diffx = (targetpos2.x) - (bpos2.x);   //target 과 bullet의 간격 계산
            float diffy = (targetpos2.y) - (bpos2.y);
            if (game_score >= 300)  // 300점 이상 달성시 success
                textPrint(text2, font, 150, 135, 80, Color::Yellow, "SUCCESS"); // 성공
            if (bullet_live[i] == 1 && fabs(diffx) < 50 && fabs(diffy) < 50) {     //간격이 어느 이하이고, 활성 bullet 이면 충돌로 간주한다.
                game_score += 10;
                sound.play();
                bullet_live[i] = 0;   //비활성으로 만든다.
                printf("\n충돌");

            }

        }
        game_score2 = to_string(game_score);
        textPrint(text1, font, 50, 135, 2,Color::White,game_score2); // 점수

        // 화면 그리기
        // Clear screen
        window.clear(sf::Color(0, 0, 0));

        // draw 모든 것
        window.draw(background);
        window.draw(scoreImg);
        window.draw(text1);
        window.draw(text2);
        window.draw(ship);
        for (int i = 0; i < 20; i++) {
            if (bullet_live[i] == 1)
                window.draw(bullet[i]);
        }

        window.draw(target);
        window.draw(background);

        // draw current frame
        window.display();

        //------------------참고로 데이터들을 콘솔 창에 출력해 본다.-------------------------
        if (n % 60 == 0) {      //1초
            printf("\n----------\n");
        }
        n++;

    }

    printf("\n-----program 실행 끝-----");


    return 0;
}