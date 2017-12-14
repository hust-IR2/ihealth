#include "RFMAS.h"

RFMAS::RFMAS(void)
{
	init();
	Reset();
}

RFMAS::~RFMAS(void)
{
}


void RFMAS::Next()
{
	if (m_questions.size() > m_current_index) {
		m_current_index++;
	}
}

void RFMAS::Prev()
{
	if (m_current_index > 0) {
		m_current_index--;
	}
}

void RFMAS::Reset()
{
	m_current_index = 0;
	m_result.clear();
}


int RFMAS::getScore()
{
	int score = 0;
	std::map<int, int>::iterator begin = m_result.begin();
	for (; begin != m_result.end(); begin++) {
		score += begin->second + 1;	
	}

	return score;
}

void RFMAS::saveAnswer(int i , int index)
{
	m_result[i] = index;
}

void RFMAS::getQuestionAndAnswer(std::vector<std::wstring>& questions, std::vector<std::wstring>& answers)
{
	std::wstring question;
	std::wstring answer;

	std::map<int, int>::iterator begin = m_result.begin();
	for (; begin != m_result.end(); begin++) {
		question = getQuestion(begin->first);
		answer = getAnswers(begin->first).at(begin->second);

		questions.push_back(question);
		answers.push_back(answer);
	}
}

std::wstring RFMAS::getQuestion(int i)
{
	return m_questions.at(i);
}

std::vector<std::wstring> RFMAS::getAnswers(int i)
{
	return m_answers.at(i);
}

void RFMAS::init()
{
	m_questions.push_back(_T("从仰卧位到健侧卧位"));
	std::vector<std::wstring> answers;
	answers.push_back(_T("1.自己牵拉侧卧(起始位必须仰卧,不屈膝,患者自己用健侧手牵拉向患侧卧,用健腿帮助患腿移动)"));
	answers.push_back(_T("2.下肢主动横移且下半身随之移动(起始位同上,上肢留在后面) "));
	answers.push_back(_T("3.用健侧上肢将患侧上肢提过身体,下肢主动移动且身体随其移动。(起始位同上)"));
	answers.push_back(_T("4.患侧上肢主动移动到对侧，身体其他部位随之运动。(起始位同上) "));
	answers.push_back(_T("5.移动上下肢并翻身至侧位，但平衡差。(起始位同上，肩前伸，上肢前屈)"));
	answers.push_back(_T("6.在3s内翻身侧卧。(起始位同上，不用手) "));
	m_answers.push_back(answers);

	m_questions.push_back(_T("从仰卧位到床边坐"));
	answers.clear();
	answers.push_back(_T("1.侧卧，头侧抬起，但不坐起(帮助患者侧卧)"));
	answers.push_back(_T("2.从侧卧到床边坐。(帮助患者移动，整个过程患者能控制头部姿势)"));
	answers.push_back(_T("3.从侧卧到床边坐。(准备随时帮助将患者下肢移至床边)"));
	answers.push_back(_T("4.从侧卧到床边坐。(不需要帮助)"));
	answers.push_back(_T("5.仰卧到床边坐。(不需要帮助)"));
	answers.push_back(_T("6.在10s内从仰卧到床边坐。(不需要帮助)"));
	m_answers.push_back(answers);

	m_questions.push_back(_T("坐位平衡"));
	answers.clear();
	answers.push_back(_T("1.必须有支持才能坐帮助患者坐起"));
	answers.push_back(_T("2.无支持能坐10s（不用扶持，双膝和双足靠拢，双足可着地靠拢）"));
	answers.push_back(_T("3.无支持能坐，体重能很好的前移，且分配均匀（体重在双髋处能很好的前移，头胸伸展，两侧均匀持重）"));
	answers.push_back(_T("4.无支持能坐并能转动头和躯干向后看（双足着地支持，不让双腿外展或双足移动，双手放在大腿上，不要移动到椅座上）"));
	answers.push_back(_T("5.无支持能坐且向前触地面并返回原位（双足着地，不允许患者捉住东西，腿和双足不要移动，必要时支持患臂，手至少必须触到足前10cm的地面）"));
	answers.push_back(_T("6.无支持能坐在凳子上，触摸侧方地面并返回原位（要求姿势同上，但患者必须向侧位而不是向前方触摸）"));
	m_answers.push_back(answers);

	m_questions.push_back(_T("从坐到站"));
	answers.clear();
	answers.push_back(_T("1.需要别人帮助站起（任何方法）"));
	answers.push_back(_T("2.可在别人准备随时帮助下站起（体重分布不均，用手扶持）"));
	answers.push_back(_T("3.可以站起（不允许体重分布不均和用手扶持）"));
	answers.push_back(_T("4.可以站起，并伸直髋和膝维持5秒（不允许体重分布不均）"));
	answers.push_back(_T("5.坐—站—坐不需别人准备随时帮助（不允许体重分布不均，完全伸直髋和膝）"));
	answers.push_back(_T("6.坐—站—坐不需别人准备随时帮助，并在10秒内重复三次（不允许体重分布不均）"));
	m_answers.push_back(answers);

	m_questions.push_back(_T("步行"));
	answers.clear();
	answers.push_back(_T("1.能用患腿站，另一腿向前迈步（负重的髋关节必须伸展，可准备随时给予帮助）"));
	answers.push_back(_T("2.在一个人准备随进给以帮助下能行走"));
	answers.push_back(_T("3.不需帮助能独立行走（或借助任何辅助器具）3m"));
	answers.push_back(_T("4.不用辅助器具15s内能独立步行5m"));
	answers.push_back(_T("5.不用辅助器具25s内能独立步行10m，然后转身，拾起地上一个小沙袋，可以用任一只手，并且走回原地"));
	answers.push_back(_T("6.35s内上下四级台阶3次（不用或用辅助器具，但不能扶栏杆）"));
	m_answers.push_back(answers);

	m_questions.push_back(_T("上肢功能"));
	answers.clear();
	answers.push_back(_T("1.卧位，上举上肢以伸展肩关节（帮助前臂置于所要求的位置并给予支持，使肘伸直）"));
	answers.push_back(_T("2.卧位，保持上举伸直的上肢2s（帮助将上肢置于所要求的位置，患者必须使上肢稍外旋，肘必须伸直在20度范围内）"));
	answers.push_back(_T("3.上肢体位同2，屈伸肘部使手掌及时离开前额（可以帮助前臂旋后） "));
	answers.push_back(_T("4.坐位，使上肢伸直前屈90度保持2s（保持上肢稍外旋及伸肘，不允许过分耸肩）"));
	answers.push_back(_T("5.坐位，患者举臂同4，前屈90度，并维持10s然后还原（患者必须维持上肢稍外旋，不允许内旋）"));
	answers.push_back(_T("6.站立，手抵墙，当身体转向墙时要维持上肢的位置（上肢外展90°，手掌平压在墙上）"));
	m_answers.push_back(answers);

	m_questions.push_back(_T("手的运动"));
	answers.clear();
	answers.push_back(_T("1.坐位，伸腕（让患者坐在桌旁，前臂置于桌上，把圆柱体放在患者掌中，要求患者伸腕， 将手中的物体举离桌面，不允许屈肘）"));
	answers.push_back(_T("2.坐位，腕部桡侧偏移（将患者前臂尺侧靠放，放在旋前旋后的中位，拇指与前臂成一直线，，伸腕，手握圆柱体，然后要求患者将手抬离桌面，不允许屈肘或旋前）"));
	answers.push_back(_T("3.坐位，肘置于身旁，旋前和旋后（肘不要支持，并处直角位，3∕4的范围即可）"));
	answers.push_back(_T("4.手前伸，用双手捡起一直径14cm的大球，并把它放下（球应该放在桌上离患者较远的地方，使患者完全伸直双臂才能拿到球，肩必须前伸，双肘伸直，腕中立或伸直，双掌要接触球）"));
	answers.push_back(_T("5.从桌上拿起一个塑料杯，并把它放在身体另一侧的桌上（不能改变杯子的形态）"));
	answers.push_back(_T("6.连续用拇指和每一个手指对指，10s内做14次以上（从食指开始，每个手指依次碰拇指，不许拇指从一个手指滑向另一个手指或向回碰）"));
	m_answers.push_back(answers);

	m_questions.push_back(_T("手的精细动作"));
	answers.clear();
	answers.push_back(_T("1.捡起一个钢笔帽，再放下（患者向前伸臂，捡起笔帽放在靠近身体的桌面上）"));
	answers.push_back(_T("2.从杯子里捡出一颗糖豆，然后放在另一个杯子里（杯子里有8颗糖豆，两个杯子必须放在上肢能伸到处，左手拿右侧杯里的豆放在左侧杯里）"));
	answers.push_back(_T("3.画几条水平线止于垂直线上，20s画上10次（至少要有5条线碰到及终止在垂直线上）"));
	answers.push_back(_T("4.用一只铅笔在纸上连续迅速的点点（患者至少在每秒钟点两个点，连续5s，必须像写字一样拿笔，点点不是敲）"));
	answers.push_back(_T("5.把一匙液体放入口中（不许低头去迎就匙，不许液体溢出）"));
	answers.push_back(_T("6.用梳子梳头后部的头发"));
	m_answers.push_back(answers);

	m_questions.push_back(_T("全身肌张力"));
	answers.clear();
	answers.push_back(_T("1.迟缓无力，移动身体部分时无阻力。"));
	answers.push_back(_T("2.移动身体部分时可感觉到一些反应。"));
	answers.push_back(_T("3.变化不定，有时迟缓无力，有时肌张力正常，有时张力高。"));
	answers.push_back(_T("4.持续正常状态。"));
	answers.push_back(_T("5.50﹪时间肌张力高。"));
	answers.push_back(_T("6.肌张力持续性增高。"));
	m_answers.push_back(answers);
}

RFFMA::RFFMA()
{
	init();
}

RFFMA::~RFFMA()
{

}

void RFFMA::init()
{
	m_current_index = 0;
	m_questions.push_back(_T("肱二头肌"));
	std::vector<std::wstring> answers;
	answers.push_back(_T("不能引起反射活动"));
	answers.push_back(_T("/"));
	answers.push_back(_T("能引起反射活动"));
	m_answers.push_back(answers);

	m_questions.push_back(_T("肱三头肌"));
	answers.clear();
	answers.push_back(_T("不能引起反射活动"));
	answers.push_back(_T("/"));
	answers.push_back(_T("能引起反射活动"));
	m_answers.push_back(answers);

	m_questions.push_back(_T("肩上提"));
	answers.clear();
	answers.push_back(_T("完全不能进行"));
	answers.push_back(_T("部分完成"));
	answers.push_back(_T("无停顿地充分完成"));
	m_answers.push_back(answers);

	m_questions.push_back(_T("肩后缩"));
	answers.clear();
	answers.push_back(_T("完全不能进行"));
	answers.push_back(_T("部分完成"));
	answers.push_back(_T("无停顿地充分完成"));
	m_answers.push_back(answers);

	m_questions.push_back(_T("肩外展≥90度"));
	answers.clear();
	answers.push_back(_T("完全不能进行"));
	answers.push_back(_T("部分完成"));
	answers.push_back(_T("无停顿地充分完成"));
	m_answers.push_back(answers);

	m_questions.push_back(_T("肩外旋"));
	answers.clear();
	answers.push_back(_T("完全不能进行"));
	answers.push_back(_T("部分完成"));
	answers.push_back(_T("无停顿地充分完成"));
	m_answers.push_back(answers);

	m_questions.push_back(_T("肘屈曲"));
	answers.clear();
	answers.push_back(_T("完全不能进行"));
	answers.push_back(_T("部分完成"));
	answers.push_back(_T("无停顿地充分完成"));
	m_answers.push_back(answers);

	m_questions.push_back(_T("前臂旋后"));
	answers.clear();
	answers.push_back(_T("完全不能进行"));
	answers.push_back(_T("部分完成"));
	answers.push_back(_T("无停顿地充分完成"));
	m_answers.push_back(answers);

	m_questions.push_back(_T("肩内收、内旋"));
	answers.clear();
	answers.push_back(_T("完全不能进行"));
	answers.push_back(_T("部分完成"));
	answers.push_back(_T("无停顿地充分完成"));
	m_answers.push_back(answers);

	m_questions.push_back(_T("肘伸展"));
	answers.clear();
	answers.push_back(_T("完全不能进行"));
	answers.push_back(_T("部分完成"));
	answers.push_back(_T("无停顿地充分完成"));
	m_answers.push_back(answers);

	m_questions.push_back(_T("前臂旋前"));
	answers.clear();
	answers.push_back(_T("完全不能进行"));
	answers.push_back(_T("部分完成"));
	answers.push_back(_T("无停顿地充分完成"));
	m_answers.push_back(answers);

	m_questions.push_back(_T("手触腰椎"));
	answers.clear();
	answers.push_back(_T("没有明显活动"));
	answers.push_back(_T("手仅可向后越过髂前上棘"));
	answers.push_back(_T("能顺利完成"));
	m_answers.push_back(answers);

	m_questions.push_back(_T("肩关节屈曲90度，前臂旋前、旋后"));
	answers.clear();
	answers.push_back(_T("开始时手臂立即外展或肘关节屈曲"));
	answers.push_back(_T("在接近规定位置时肩关节外展或肘关节屈曲"));
	answers.push_back(_T("能顺利充分完成"));
	m_answers.push_back(answers);

	m_questions.push_back(_T("肩0度，屈肘90度,前臂旋前、旋后"));
	answers.clear();
	answers.push_back(_T("不能屈肘或前臂,不能旋前"));
	answers.push_back(_T("肩、肘位正确，基本能旋前、旋后"));
	answers.push_back(_T("顺利完成"));
	m_answers.push_back(answers);

	m_questions.push_back(_T("肩关节外展90度，肘伸直，前臂旋前 "));
	answers.clear();
	answers.push_back(_T("开始时肘就屈曲，前臂偏离方向不能旋前"));
	answers.push_back(_T("部分完成动作或肘关节屈曲或前臂不能旋前"));
	answers.push_back(_T("/"));
	m_answers.push_back(answers);

	m_questions.push_back(_T("肩关节前屈举臂过头肘伸直前臂中立位"));
	answers.clear();
	answers.push_back(_T("开始时肘关节屈曲或肩关节外展"));
	answers.push_back(_T("肩屈曲中途，肘关节屈曲，肩关节外展"));
	answers.push_back(_T("/"));
	m_answers.push_back(answers);

	m_questions.push_back(_T("肩屈曲30度－90度肘伸直前臂旋前旋后"));
	answers.clear();
	answers.push_back(_T("前臂旋前旋后完全不能或肩肘位不正确"));
	answers.push_back(_T("肩肘位置正确基本能完成旋前旋后"));
	answers.push_back(_T("/"));
	m_answers.push_back(answers);

	m_questions.push_back(_T("查肱二头肌、肱三头肌、指屈肌3反射"));
	answers.clear();
	answers.push_back(_T("至少2－3个反射明显亢进"));
	answers.push_back(_T("1个反射明显亢进或至少2个反射活跃"));
	answers.push_back(_T("活跃反射≤1个,且无反射亢进"));
	m_answers.push_back(answers);

	m_questions.push_back(_T("肩0度，肘屈90度腕背屈"));
	answers.clear();
	answers.push_back(_T("不能背屈腕关节达15度"));
	answers.push_back(_T("可完成腕背屈，但不能抗拒阻力"));
	answers.push_back(_T("施加轻微阻力仍可保持腕背屈"));
	m_answers.push_back(answers);

	m_questions.push_back(_T("肩0度，肘屈90度腕屈伸"));
	answers.clear();
	answers.push_back(_T("不能随意屈伸"));
	answers.push_back(_T("不能在全关节范围内主动活动腕关节"));
	answers.push_back(_T("不停顿进行"));
	m_answers.push_back(answers);

	m_questions.push_back(_T("腕背屈"));
	answers.clear();
	answers.push_back(_T("不能背屈腕关节达15度"));
	answers.push_back(_T("可完成腕背屈，但不能抗拒阻力"));
	answers.push_back(_T("施加轻微阻力可保持腕背屈"));
	m_answers.push_back(answers);

	m_questions.push_back(_T("腕屈伸"));
	answers.clear();
	answers.push_back(_T("不能随意屈伸"));
	answers.push_back(_T("不能在全关节范围内主动活动腕关节"));
	answers.push_back(_T("能平滑不停顿的进行"));
	m_answers.push_back(answers);

	m_questions.push_back(_T("腕环行运动"));	
	answers.clear();
	answers.push_back(_T("不能进行"));
	answers.push_back(_T("活动费力或不完全"));
	answers.push_back(_T("正常完成"));
	m_answers.push_back(answers);

	m_questions.push_back(_T("集团屈曲"));
	answers.clear();
	answers.push_back(_T("不能屈曲"));
	answers.push_back(_T("能屈曲但不充分能放松主动屈曲的手指"));
	answers.push_back(_T("能完成主动屈曲和伸展"));
	m_answers.push_back(answers);

	m_questions.push_back(_T("集团伸展"));
	answers.clear();
	answers.push_back(_T("不能伸展"));
	answers.push_back(_T("能放松主动屈曲的手指 "));
	answers.push_back(_T("能完全主动伸展"));
	m_answers.push_back(answers);

	m_questions.push_back(_T("钩状抓握"));
	answers.clear();
	answers.push_back(_T("不能保持要求位置"));
	answers.push_back(_T("握力微弱"));
	answers.push_back(_T("能抵抗相当大阻力"));
	m_answers.push_back(answers);

	m_questions.push_back(_T("侧捏"));
	answers.clear();
	answers.push_back(_T("完全不能 "));
	answers.push_back(_T("握力微弱"));
	answers.push_back(_T("能抵抗相当大阻力"));
	m_answers.push_back(answers);

	m_questions.push_back(_T("圆柱状抓握"));
	answers.clear();
	answers.push_back(_T("不能保持要求位置"));
	answers.push_back(_T("握力微弱"));
	answers.push_back(_T("能抵抗相当大阻力"));
	m_answers.push_back(answers);

	m_questions.push_back(_T("球形抓握"));
	answers.clear();
	answers.push_back(_T("不能保持要求位置"));
	answers.push_back(_T("握力微弱"));
	answers.push_back(_T("能抵抗相当大阻力"));
	m_answers.push_back(answers);

	m_questions.push_back(_T("震颤"));
	answers.clear();
	answers.push_back(_T("明显震颤"));
	answers.push_back(_T("轻度震颤"));
	answers.push_back(_T("无震颤"));
	m_answers.push_back(answers);

	m_questions.push_back(_T("辨距障碍"));
	answers.clear();
	answers.push_back(_T("明显或不规则"));
	answers.push_back(_T("轻度或规则"));
	answers.push_back(_T("无"));
	m_answers.push_back(answers);

	m_questions.push_back(_T("速度"));
	answers.clear();
	answers.push_back(_T("较健侧长6秒"));
	answers.push_back(_T("较健侧长2－5秒"));
	answers.push_back(_T("两侧差别<2秒"));
	m_answers.push_back(answers);
}

void RFFMA::Next()
{
	if (m_questions.size() > m_current_index) {
		m_current_index++;
	}
}

void RFFMA::Prev()
{
	if (m_current_index > 0) {
		m_current_index--;
	}
}

void RFFMA::Reset()
{
	m_current_index = 0;
	m_result.clear();
}

int RFFMA::getScore()
{
	int score = 0;
	std::map<int, int>::iterator begin = m_result.begin();
	for (; begin != m_result.end(); begin++) {
		score += begin->second + 1;	
	}

	return score;
}

void RFFMA::saveAnswer(int i , int index)
{
	m_result[i] = index;
}


void RFFMA::getQuestionAndAnswer(std::vector<std::wstring>& questions, std::vector<std::wstring>& answers)
{
	std::wstring question;
	std::wstring answer;

	std::map<int, int>::iterator begin = m_result.begin();
	for (; begin != m_result.end(); begin++) {
		question = getQuestion(begin->first);
		answer = getAnswers(begin->first).at(begin->second);

		questions.push_back(question);
		answers.push_back(answer);
	}
}

std::wstring RFFMA::getQuestion(int i)
{
	return m_questions.at(i);
}

std::vector<std::wstring> RFFMA::getAnswers(int i)
{
	return m_answers.at(i);
}