//---------------------------------------------------------------
//
// Label.h
//

#pragma once

#include "client/RenderableImpl.h"

#include <memory>
#include <string>

namespace Client {

class Label : public RenderableImpl {
public:
	Label();
	Label(const std::string& text);
	virtual ~Label();

	void Initialize();

	void SetText(const std::string& text);
	const std::string& GetText() const;

	// RenderableImpl impl
	virtual void Render() const override;

private:
	void UpdateTextureData();

private:
	std::string m_text;
	SDL_Rect rect{0, 0, 0, 0};
};

} // namespace Ui
