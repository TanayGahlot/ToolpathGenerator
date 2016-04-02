class ToolConfig{
	public:
		float toolDiameter;
		float toolLength;
		float spindleDiameter;
		float stepSize;
		float overlap;
		float feedRate;
		float safeHeight;
		ToolConfig(float tD = 1, float sS = 1, float oL = 1, float fR = 500, float tL = 10, float sD = 200, float sH = 0){
			toolDiameter = tD;
			toolLength = tL;
			spindleDiameter = sD;
			stepSize = sS;
			overlap = oL;
			feedRate = fR;
			safeHeight = sH;
		}
};
