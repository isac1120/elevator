// C++ code
//

const int FLOOR_NUM = 3;
const int STEP_NUM = 7;
const int BTNS[FLOOR_NUM] = {11,12,13};
const int FLOOR_LEDS[FLOOR_NUM] = {8,9,10};
const int STEP_LEDS[STEP_NUM] = {A0,2,3,4,5,6,7};
bool btn_state[FLOOR_NUM] = {false,false,false};
bool btn_last_state[FLOOR_NUM] = {false,false,false};
bool request[FLOOR_NUM] = {false,false,false};
int current_index=0;
long prev = 0;
long interval = 500;
int target_index =0;
bool is_down=false;
bool is_empty=false;
bool is_arrived = false;    
unsigned long arrived_time = 0;
long stop_duration = 2000;  
void setup()
{
  Serial.begin(9600);
  //BTNS은 INPUT으로, FLOOR_LEDS는 OUTPUT으로 사용
  for(int i = 0; i < FLOOR_NUM; i++)
  {
    pinMode(BTNS[i],INPUT);
    pinMode(FLOOR_LEDS[i],OUTPUT);
  }
  //STEP_LEDS는 OUTPUT으로 사용
  for(int i = 0; i < STEP_NUM; i++)
  {
    pinMode(STEP_LEDS[i],OUTPUT);
  }
  digitalWrite(STEP_LEDS[0],HIGH);
}
//btn 입력에 따라 층별request를 저장하고, FLOOR_LED를 제어하는 함수
void handleButtonInput() {
  //버튼 state 읽어오기
  for(int i = 0; i < FLOOR_NUM; i++) {
    btn_state[i] = digitalRead(BTNS[i]);
    if(btn_last_state[i] == false && btn_state[i] == true) 
    {
      request[i] = !request[i];
      digitalWrite(FLOOR_LEDS[i], request[i] ? HIGH : LOW);
    }
    btn_last_state[i] = btn_state[i];
  }

}

// request 상태에 따라 목표 STEP을 설정하는 함수
void updateTargetIndex() {
  is_empty = true;

  int target_floor = -1;
  	
  //내려가는 중인 경우
  if(is_down)
  {
    //request가 존재하는 층이 있다면, 그 중 현재층 이하의 최상층을 가져옴 
    for( int i = 0; i <= current_index/3; i++)
    {
      if (request[i]) 
      {
        target_floor = i;
        is_empty = false;
      }
    }
  }
  //정지상태거나 올라가는 중인 경우
  else
  {	//request가 존재하는 층이 있다면, 그 중 최상층을 가져옴
    for (int i = 0; i < FLOOR_NUM; i++) 
    {
      if (request[i]) {
        target_floor = i;
        is_empty = false;
      }
    }
    
  }
	//request가 존재하는 층이 있는 경우
  if (!is_empty) {
    // 요청된 층 중 최상층으로 target 설정
    target_index = target_floor * 3;
    //request가 존재하는 층이 없는 경우
  } else {
    // 요청이 없으면 현재 위치 기준 아래층으로 설정
    target_index = (current_index / 3) * 3;
  }
}


void moveElevator() {
    // 도착해서 멈춰있는 상태라면 일정 시간 대기
  if (is_arrived) {
    if (millis() - arrived_time < stop_duration) {
      return; // 아직 대기 중
    } else {
      is_arrived = false; // 대기 끝났으니 다음 이동 가능
    }
  }
  // 모든 STEP LED 끄기
  for (int i = 0; i < STEP_NUM; i++) {
    digitalWrite(STEP_LEDS[i], LOW);
  }

  // 엘리베이터 이동 처리
  //올라갈 때
  if (current_index < target_index) {
    current_index++;
    is_down=false;
  } 
  //내려갈 떄
  else if (current_index > target_index) {
    current_index--;
    is_down=true;
  }
  // 도착했을 경우 층 요청 처리
  else 
  {
    int floor = current_index / 3;
    request[floor] = false;
    digitalWrite(FLOOR_LEDS[floor], LOW);
    is_down = false;
  
    is_arrived = true;
    arrived_time = millis(); 
  }
  // 현재 위치 LED만 켜기
  digitalWrite(STEP_LEDS[current_index], HIGH);
}
void loop()
{
	//입력된 btn 에 따라 request를 기록하고, 클릭된 층의 FLOOR_LED를 점등
  handleButtonInput();
  
  //request에 따라 목표 STEP을 설정
  updateTargetIndex();

  //interval마다 목표STEP을 향해 엘리베이터를 이동 및 request를 처리하고 FLOOR_LED를 소등
  if(millis() - prev > interval)
  {
    prev = millis();
	moveElevator();
    
  }
	delay(100);
}
