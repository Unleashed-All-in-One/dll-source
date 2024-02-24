BB_SET_OBJECT_MAKE_HOOK(ETFStageGate);


ETFStageGateUIContainer* ETFStageGate::containerUI;
void ETFStageGate::registerObject()
{
    BB_INSTALL_SET_OBJECT_MAKE_HOOK(ETFStageGate)
}