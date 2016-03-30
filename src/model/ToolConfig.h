class ToolConfig{
	public:
		float toolDiameter;
		float toolLength;
		float spindleDiameter;
		float stepSize;
		float overlap;
		float feedRate;
		ToolConfig(float tD = 1, float sS = 1, float oL, float fR = 500, float tL = 8, float sD = 200){
			toolDiameter = tD;
			toolLength = tL;
			spindleDiameter = sD;
			stepSize = sS;
			overlap = oL;
			feedRate = fR;
		}
};
