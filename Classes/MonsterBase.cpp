#include "MonsterBase.h"
//��ʼ��
bool MonsterBase::init()
{
    if (!Sprite::init())
    {
        return false;
    }
    return true;
}
//�������� ����
Animation* MonsterBase::createAnimation(std::string prefixName, int framesNum, float delay)
{
    Vector<SpriteFrame*> animFrames;
    for (int i = 1; i <= framesNum; i++) {
        char buffer[20] = { 0 };
        sprintf(buffer, "%02d.png", i);
        std::string str = prefixName + buffer;

        auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(str);
        if (frame != nullptr) {
            animFrames.pushBack(frame);
        }
    }
    return Animation::createWithSpriteFrames(animFrames, delay);
}
//�ı䷽��
void MonsterBase::changeDirection(float dt)
{
    const auto curr = currPoint();

    if (curr->getPositionX() > this->getPosition().x) {
        runAction(Animate::create(AnimationCache::getInstance()->getAnimation("runright")));
    }
    else {
        runAction(Animate::create(AnimationCache::getInstance()->getAnimation("runleft")));
    }

    if (curr->getPositionY() > this->getPosition().y) {
        runAction(Animate::create(AnimationCache::getInstance()->getAnimation("runup")));
    }
    else {
        runAction(Animate::create(AnimationCache::getInstance()->getAnimation("rundown")));
    }
}
//���ڵ�λ��
Node* MonsterBase::currPoint()
{
    return this->pointsVector.at(pointCounter);
}

Node* MonsterBase::nextPoint()
{
    const int maxCount = this->pointsVector.size();
    pointCounter++;
    if (pointCounter < maxCount) {
        auto node = this->pointsVector.at(pointCounter);
        return node;
    }
    else {
        pointCounter = maxCount - 1;
    }
    return nullptr;
}

void MonsterBase::runFllowPoint()
{
    auto point = currPoint();
    setPosition(point->getPosition());
    point = nextPoint();
    if (point != NULL) {
        runAction(CCSequence::create(MoveTo::create(getRunSpeed(), point->getPosition()), CallFuncN::create(CC_CALLBACK_0(MonsterBase::runFllowPoint, this)), NULL));
    }
}

void MonsterBase::setPointsVector(Vector<Node*> points)
{
    // �������·����
    pointsVector.clear();

    // ����·���������
    pointCounter = 0;

    // �����µ�·����
    for (auto point : points) {
        pointsVector.pushBack(point);
    }

    // �����·����,��ʼ�ƶ�
    if (!pointsVector.empty()) {
        runFllowPoint();
    }
}

void MonsterBase::playAnimation(const std::string& animName)
{
    // ֹͣ��ǰ����
    stopAllActions();

    // �Ӷ��������ȡ����
    auto animation = AnimationCache::getInstance()->getAnimation(animName);
    if (animation) {
        // ������������������
        auto animate = Animate::create(animation);
        runAction(animate);
    }
}
