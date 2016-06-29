#ifndef EDGEDETECTOR_HEADER_
#define EDGEDETECTOR_HEADER_

class EdgeDetector {
public:
	explicit EdgeDetector(int border = 5) : border_(border)
	{
		reset();
	}

	void update(int x, int y, int w, int h)
	{
		xoff_ = 0;
		yoff_ = 0;

		onLeftEdge_ = (x >= 0 && x < border_ && y >= 0 && y < h);
		if (onLeftEdge_) {
			xoff_ = x;
		}

		onRightEdge_ = (x >= w - border_ && x < w && y >= 0 && y < h);
		if (onRightEdge_) {
			xoff_ = w - x - 1;
		}

		onTopEdge_ = (y >= 0 && y < border_ && x >= 0 && x < w);
		if (onTopEdge_) {
			yoff_ = y;
		}

		onBottomEdge_ = (y >= h - border_ && y < h && x >= 0 && x < w);
		if (onBottomEdge_) {
			yoff_ = h - y - 1;
		}
	}

	void reset()
	{
		onLeftEdge_ = false;
		onRightEdge_ = false;
		onTopEdge_ = false;
		onBottomEdge_ = false;

		xoff_ = 0;
		yoff_ = 0;
	}

	void setBorder(int border)
	{
		border_ = border;
	}

	int border() const
	{
		return border_;
	}

	bool onLeftEdge() const
	{
		return onLeftEdge_;
	}

	bool onRightEdge() const
	{
		return onRightEdge_;
	}

	bool onTopEdge() const
	{
		return onTopEdge_;
	}

	bool onBottomEdge() const
	{
		return onBottomEdge_;
	}

	bool onTopLeftEdge() const
	{
		return onTopEdge_ && onLeftEdge_;
	}

	bool onBottomLeftEdge() const
	{
		return onBottomEdge_ && onLeftEdge_;
	}

	bool onTopRightEdge() const
	{
		return onTopEdge_ && onRightEdge_;
	}

	bool onBottomRightEdge() const
	{
		return onBottomEdge_ && onRightEdge_;
	}

	bool onEdges() const
	{
		return onLeftEdge_ || onRightEdge_ || onTopEdge_ || onBottomEdge_;
	}

	int xOffset() const
	{
		return xoff_;
	}

	int yOffset() const
	{
		return yoff_;
	}

private:
	int border_;
	bool onLeftEdge_;
	bool onRightEdge_;
	bool onTopEdge_;
	bool onBottomEdge_;
	int xoff_;
	int yoff_;
};

#endif // EDGEDETECTOR_HEADER_

