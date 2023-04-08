/*
 * Copyright 2016 Nu-book Inc.
 */
// SPDX-License-Identifier: Apache-2.0

#include "ReadBarcode.h"

#include <emscripten/bind.h>
#include <memory>
#include <stdexcept>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

struct ReadResult
{
	std::string format{};
	std::string text{};
	std::string error{};
	std::string eccLevel{};
	std::string version{};
	int orientation;
	bool isMirrored;
	bool isInverted;
	ZXing::Position position{};
};

using ReadResults = std::vector<ReadResult>;

ReadResults readBarcodeFromImageView(ZXing::ImageView iv, bool tryHarder, const std::string &formats, int maxNumberOfSymbols)
{
	using namespace ZXing;
	ReadResults readResults;
	try
	{
		DecodeHints hints;
		hints.setTryHarder(tryHarder);
		hints.setTryRotate(tryHarder);
		hints.setTryInvert(tryHarder);
		hints.setTryDownscale(tryHarder);
		hints.setFormats(BarcodeFormatsFromString(formats));
		hints.setMaxNumberOfSymbols(maxNumberOfSymbols);

		auto results = ReadBarcodes(iv, hints);
		readResults.resize(results.size());
		std::transform(results.begin(), results.end(), readResults.begin(), [](auto &result)
									 { return ReadResult{
												 ToString(result.format()),
												 result.text(),
												 "",
												 result.ecLevel(),
												 result.version(),
												 result.orientation(),
												 result.isMirrored(),
												 result.isInverted(),
												 result.position()}; });
		return readResults;
	}
	catch (const std::exception &e)
	{
		readResults.push_back({"", "", e.what()});
		return readResults;
	}
	catch (...)
	{
		readResults.push_back({"", "", "Unknown Error"});
		return readResults;
	}
	return readResults;
}

ReadResults readBarcodeFromImage(int bufferPtr, int bufferLength, bool tryHarder, std::string formats, int maxNumberOfSymbols)
{
	using namespace ZXing;

	int width, height, channels;
	std::unique_ptr<stbi_uc, void (*)(void *)> buffer(
			stbi_load_from_memory(reinterpret_cast<const unsigned char *>(bufferPtr), bufferLength, &width, &height, &channels, 1),
			stbi_image_free);
	if (buffer == nullptr)
	{
		return ReadResults{{"", "", "Error loading image"}};
	}

	return readBarcodeFromImageView({buffer.get(), width, height, ImageFormat::Lum}, tryHarder, formats, maxNumberOfSymbols);
}

ReadResults readBarcodeFromPixmap(int bufferPtr, int imgWidth, int imgHeight, bool tryHarder, std::string formats,
																	int maxNumberOfSymbols)
{
	using namespace ZXing;
	return readBarcodeFromImageView({reinterpret_cast<uint8_t *>(bufferPtr), imgWidth, imgHeight, ImageFormat::RGBX}, tryHarder,
																	formats, maxNumberOfSymbols);
}

EMSCRIPTEN_BINDINGS(BarcodeReader)
{
	using namespace emscripten;

	value_object<ReadResult>("ReadResult")
			.field("format", &ReadResult::format)
			.field("text", &ReadResult::text)
			.field("error", &ReadResult::error)
			.field("eccLevel", &ReadResult::eccLevel)
			.field("version", &ReadResult::version)
			.field("orientation", &ReadResult::orientation)
			.field("isMirrored", &ReadResult::isMirrored)
			.field("isInverted", &ReadResult::isInverted)
			.field("position", &ReadResult::position);

	register_vector<ReadResult>("ReadResults");

	value_object<ZXing::PointI>("Point").field("x", &ZXing::PointI::x).field("y", &ZXing::PointI::y);

	value_object<ZXing::Position>("Position")
			.field("topLeft", emscripten::index<0>())
			.field("topRight", emscripten::index<1>())
			.field("bottomRight", emscripten::index<2>())
			.field("bottomLeft", emscripten::index<3>());

	function("readBarcodeFromImage", &readBarcodeFromImage);
	function("readBarcodeFromPixmap", &readBarcodeFromPixmap);
};
