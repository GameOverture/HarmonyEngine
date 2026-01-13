/**************************************************************************
 *	HyMath.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Utilities/HyMath.h"

const float HyMath::FloatSlop = 0.005f;

const HyColor HyColor::White(0xffffff);
const HyColor HyColor::Black(0x000000);
const HyColor HyColor::Red(0xff0000);
const HyColor HyColor::DarkRed(0x800000);
const HyColor HyColor::Pink(0xff49ce);
const HyColor HyColor::Green(0xff00);
const HyColor HyColor::DarkGreen(0x008000);
const HyColor HyColor::Blue(0xff);
const HyColor HyColor::DarkBlue(0x000080);
const HyColor HyColor::Cyan(0xffff);
const HyColor HyColor::DarkCyan(0x008080);
const HyColor HyColor::Magenta(0xff00ff);
const HyColor HyColor::Purple(0x811CF6);
const HyColor HyColor::Yellow(0xffff00);
const HyColor HyColor::Gold(0xffd700);
const HyColor HyColor::DarkYellow(0x808000);
const HyColor HyColor::Gray(0xa0a0a4);
const HyColor HyColor::DarkGray(0x222222);
const HyColor HyColor::LightGray(0xc0c0c0);
const HyColor HyColor::Orange(0xFA5A0A);
const HyColor HyColor::Brown(0x802600);
const HyColor HyColor::PanelContainer(0x2D2D2D);
const HyColor HyColor::FrameContainer(0x202020);// 0x3F3F46);
const HyColor HyColor::PanelWidget(0x3F3F46);
const HyColor HyColor::FrameWidget(0x777777);
const HyColor HyColor::_InternalUse(0xDEADBE);

void HySetVec(glm::vec2 &vecOut, float fX, float fY)
{
	vecOut.x = fX; vecOut.y = fY;
}

void HySetVec(glm::vec2 &vecOut, const glm::vec2 &srcRef)
{
	vecOut = srcRef;
}

void HySetVec(glm::vec2 &vecOut, const glm::vec3 &srcRef)
{
	vecOut.x = srcRef.x;
	vecOut.y = srcRef.y;
}

void HySetVec(glm::vec2 &vecOut, const glm::vec4 &srcRef)
{
	vecOut.x = srcRef.x;
	vecOut.y = srcRef.y;
}

void HySetVec(glm::ivec2 &vecOut, int32 iX, int32 iY)
{
	vecOut.x = iX; vecOut.y = iY;
}

void HySetVec(glm::ivec2 &vecOut, const glm::ivec2 &srcRef)
{
	vecOut = srcRef;
}

void HySetVec(glm::ivec2 &vecOut, const glm::ivec3 &srcRef)
{
	vecOut.x = srcRef.x;
	vecOut.y = srcRef.y;
}

void HySetVec(glm::ivec2 &vecOut, const glm::ivec4 &srcRef)
{
	vecOut.x = srcRef.x;
	vecOut.y = srcRef.y;
}

void HySetVec(glm::vec3 &vecOut, float fX, float fY, float fZ)
{
	vecOut.x = fX; vecOut.y = fY; vecOut.z = fZ;
}

void HySetVec(glm::vec3 &vecOut, const glm::vec2 &srcRef)
{
	vecOut.x = srcRef.x;
	vecOut.y = srcRef.y;
	vecOut.z = 0.0f;
}

void HySetVec(glm::vec3 &vecOut, const glm::vec3 &srcRef)
{
	vecOut = srcRef;
}

void HySetVec(glm::vec3 &vecOut, const glm::vec4 &srcRef)
{
	vecOut.x = srcRef.x;
	vecOut.y = srcRef.y;
	vecOut.z = srcRef.z;
}

void HySetVec(glm::ivec3 &vecOut, int32 iX, int32 iY, int32 iZ)
{
	vecOut.x = iX; vecOut.y = iY; vecOut.z = iZ;
}

void HySetVec(glm::ivec3 &vecOut, const glm::ivec2 &srcRef)
{
	vecOut.x = srcRef.x;
	vecOut.y = srcRef.y;
	vecOut.z = 0;
}

void HySetVec(glm::ivec3 &vecOut, const glm::ivec3 &srcRef)
{
	vecOut = srcRef;
}

void HySetVec(glm::ivec3 &vecOut, const glm::ivec4 &srcRef)
{
	vecOut.x = srcRef.x;
	vecOut.y = srcRef.y;
	vecOut.z = srcRef.z;
}

void HySetVec(glm::vec4 &vecOut, float fX, float fY, float fZ, float fW)
{
	vecOut.x = fX; vecOut.y = fY; vecOut.z = fZ; vecOut.w = fW;
}

void HySetVec(glm::vec4 &vecOut, const glm::vec2 &srcRef)
{
	vecOut.x = srcRef.x;
	vecOut.y = srcRef.y;
	vecOut.z = 0.0f;
	vecOut.w = 0.0f;
}

void HySetVec(glm::vec4 &vecOut, const glm::vec3 &srcRef)
{
	vecOut.x = srcRef.x;
	vecOut.y = srcRef.y;
	vecOut.z = srcRef.z;
	vecOut.w = 0.0f;
}

void HySetVec(glm::vec4 &vecOut, const glm::vec4 &srcRef)
{
	vecOut = srcRef;
}

void HySetVec(glm::ivec4 &vecOut, int32 iX, int32 iY, int32 iZ, int32 iW)
{
	vecOut.x = iX; vecOut.y = iY; vecOut.z = iZ; vecOut.w = iW;
}

void HySetVec(glm::ivec4 &vecOut, const glm::ivec2 &srcRef)
{
	vecOut.x = srcRef.x;
	vecOut.y = srcRef.y;
	vecOut.z = 0;
	vecOut.w = 0;
}

void HySetVec(glm::ivec4 &vecOut, const glm::ivec3 &srcRef)
{
	vecOut.x = srcRef.x;
	vecOut.y = srcRef.y;
	vecOut.z = srcRef.z;
	vecOut.w = 0;
}

void HySetVec(glm::ivec4 &vecOut, const glm::ivec4 &srcRef)
{
	vecOut = srcRef;
}

glm::ivec2 HyMath::LockAspectRatio(int32 iOldWidth, int32 iOldHeight, int32 iNewWidth, int32 iNewHeight)
{
	// Preserve old aspect ratio
	float fLockedAspectRatio = static_cast<float>(iOldWidth) / static_cast<float>(iOldHeight);
	float fScaledAspectRatio = static_cast<float>(iNewWidth) / static_cast<float>(iNewHeight);

	// Determine whether width or height is our constraint, then calculate the return size
	glm::ivec2 vReturnSize(iNewWidth, iNewHeight);
	if(fScaledAspectRatio < fLockedAspectRatio) // Width constraint (calculate height, based on provided width)
	{
		vReturnSize.y = (iNewWidth * iOldHeight) / iOldWidth;
		vReturnSize.x = iNewWidth;
	}
	else if(fScaledAspectRatio > fLockedAspectRatio) // Height constraint (calculate width, based on provided height)
	{
		vReturnSize.x = iNewHeight * iOldWidth / iOldHeight;
		vReturnSize.y = iNewHeight;
	}

	return vReturnSize;
}

/*static*/ void HyMath::InvalidateAABB(b2AABB &aabbOut)
{
	aabbOut.lowerBound = { 1.0f, 1.0f };
	aabbOut.upperBound = { -1.0f, -1.0f };
}

/*static*/ bool HyMath::TestPointAABB(const b2AABB &aabb, const glm::vec2 &pt)
{
	return (b2IsValidAABB(aabb) &&
		pt.x >= aabb.lowerBound.x && pt.y >= aabb.lowerBound.y &&
		pt.x <= aabb.upperBound.x && pt.y <= aabb.upperBound.y);
}

/*static*/ bool HyMath::TestOverlapAABB(const b2AABB &a, const b2AABB &b)
{
	return !(b.lowerBound.x > a.upperBound.x || b.lowerBound.y > a.upperBound.y || a.lowerBound.x > b.upperBound.x || a.lowerBound.y > b.upperBound.y);
}

/*static*/ glm::vec2 HyMath::PerpendicularClockwise(const glm::vec2 &vDirVector)
{
	return glm::vec2(vDirVector.y, -vDirVector.x);
}

/*static*/ glm::ivec2 HyMath::PerpendicularClockwise(const glm::ivec2 &vDirVector)
{
	return glm::ivec2(vDirVector.y, -vDirVector.x);
}

/*static*/ glm::vec2 HyMath::PerpendicularCounterClockwise(const glm::vec2 &vDirVector)
{
	return glm::vec2(-vDirVector.y, vDirVector.x);
}

/*static*/ glm::ivec2 HyMath::PerpendicularCounterClockwise(const glm::ivec2 &vDirVector)
{
	return glm::ivec2(-vDirVector.y, vDirVector.x);
}

/*static*/ float HyMath::CrossProduct(const glm::vec2 &vA, const glm::vec2 &vB)
{
	return (vA.x * vB.y) - (vA.y * vB.x);
}

/*static*/ float HyMath::WindingOrder(const glm::vec2 &ptA, const glm::vec2 &ptB, const glm::vec2 &ptC)
{
	return CrossProduct(ptB - ptA, ptC - ptA);
}

/*static*/ bool HyMath::IsConvexPolygon(const std::vector<glm::vec2> &ccwOrderedVertexList)
{
	const int iNumVerts = static_cast<int>(ccwOrderedVertexList.size());
	if(iNumVerts < 3)
		return false;

	for(int i = 0; i < iNumVerts; ++i)
	{
		const glm::vec2 &pt1Ref = ccwOrderedVertexList[i];
		const glm::vec2 &pt2Ref = ccwOrderedVertexList[(i + 1) % iNumVerts];
		const glm::vec2 &pt3Ref = ccwOrderedVertexList[(i + 2) % iNumVerts];

		if(WindingOrder(pt1Ref, pt2Ref, pt3Ref) <= 0.0f)
			return false;
	}
}

/*static*/ bool HyMath::TestPointTriangle(const glm::vec2 &ptA, const glm::vec2 &ptB, const glm::vec2 &ptC, const glm::vec2 &ptTest)
{
	float f1 = WindingOrder(ptA, ptB, ptTest);
	float f2 = WindingOrder(ptB, ptC, ptTest);
	float f3 = WindingOrder(ptC, ptA, ptTest);
	return (f1 >= 0.0f && f2 >= 0.0f && f3 >= 0.0f);
}

/*static*/ bool HyMath::TestPointTriangle(const HyTriangle2d &tri, const glm::vec2 &ptTest)
{
	return TestPointTriangle(tri.m_ptA, tri.m_ptB, tri.m_ptC, ptTest);
}

/*static*/ std::vector<HyTriangle2d> HyMath::Triangulate(const std::vector<glm::vec2> &ccwOrderedVertexList)
{
	std::vector<HyTriangle2d> result;
	
	std::vector<int> indices(ccwOrderedVertexList.size());
	for(int i = 0; i < (int)ccwOrderedVertexList.size(); ++i)
		indices[i] = i;

	// fpIsEar - Test for triangles with two sides being the edges of the polygon and the third one completely inside it
	std::function<bool(int)> fpIsEar = 
		[&indices, ccwOrderedVertexList](int i) -> bool
		{
			int n = static_cast<int>(indices.size());
			int i0 = indices[(i + n - 1) % n];
			int i1 = indices[i];
			int i2 = indices[(i + 1) % n];

			const glm::vec2 &a = ccwOrderedVertexList[i0];
			const glm::vec2 &b = ccwOrderedVertexList[i1];
			const glm::vec2 &c = ccwOrderedVertexList[i2];
			if(WindingOrder(a, b, c) <= 0.0f)
				return false; // Must be convex

			for(int j = 0; j < n; ++j) // No other point inside
			{
				int vi = indices[j];
				if(vi == i0 || vi == i1 || vi == i2)
					continue;

				if(TestPointTriangle(a, b, c, ccwOrderedVertexList[vi]))
					return false;
			}

			return true;
		};

	while(indices.size() > 3)
	{
		bool bEarFound = false;
		for(int i = 0; i < (int)indices.size(); ++i)
		{
			if(fpIsEar(i) == false)
				continue;

			int i0 = indices[(i + indices.size() - 1) % indices.size()];
			int i1 = indices[i];
			int i2 = indices[(i + 1) % indices.size()];

			result.push_back({ ccwOrderedVertexList[i0], ccwOrderedVertexList[i1], ccwOrderedVertexList[i2] });

			indices.erase(indices.begin() + i);
			bEarFound = true;
			break;
		}

		// Degenerate case (should not happen if polygon is simple)
		if(bEarFound == false)
			break;
	}

	// Final triangle
	if(indices.size() == 3)
		result.push_back({ ccwOrderedVertexList[indices[0]], ccwOrderedVertexList[indices[1]], ccwOrderedVertexList[indices[2]] });

	return result;
}

// Caution: Operationally expensive
/*static*/ float HyMath::AngleFromVector(const glm::vec2 &vDirVector)
{
	glm::vec2 vNormalizedDirVector = glm::normalize(vDirVector);
	return glm::degrees(atan2(vNormalizedDirVector.y, vNormalizedDirVector.x));
}

/*static*/ glm::vec2 HyMath::ClosestPointOnRay(const glm::vec2 &ptRayStart, const glm::vec2 &vNormalizedRayDir, const glm::vec2 &ptTestPoint)
{
	// Get the vector from the start of the ray to the test point
	glm::vec2 v = ptTestPoint - ptRayStart;

	// Get the projection of v onto the ray direction
	float t = glm::dot(v, vNormalizedRayDir);

	// If the projection is negative, the closest point is the start of the ray
	if(t < 0.0f)
		return ptRayStart;

	// Otherwise, the closest point is along the ray
	return ptRayStart + t * vNormalizedRayDir;
}

/*static*/ glm::vec2 HyMath::ClosestPointOnSegment(const glm::vec2 &pt1, const glm::vec2 &pt2, const glm::vec2 &ptTestPoint)
{
	glm::vec2 vDirection = pt2 - pt1;
	float fLengthSq = glm::dot(vDirection, vDirection);

	// If the segment is a point, return that point
	if(fLengthSq == 0.0f)
		return pt1;

	// Project the test point onto the line defined by pt1 and pt2
	float t = glm::dot(ptTestPoint - pt1, vDirection) / fLengthSq;
	t = glm::clamp(t, 0.0f, 1.0f);

	return pt1 + (t * vDirection);
}

/*static*/ bool HyMath::TestSegmentsOverlap(const glm::vec2 &ptSegA1, const glm::vec2 &ptSegA2, const glm::vec2 &ptSegB1, const glm::vec2 &ptSegB2, glm::vec2 &ptIntersectionOut)
{
	// Calculate direction vectors for both segments
	glm::vec2 vDirA = ptSegA2 - ptSegA1;
	glm::vec2 vDirB = ptSegB2 - ptSegB1;

	// Calculate the denominator for the intersection formula
	float fDenom = vDirA.x * vDirB.y - vDirA.y * vDirB.x;

	// If the denominator is zero, the lines are parallel or collinear
	if(std::abs(fDenom) < HyMath::FloatSlop)
	{
		// Check if the segments are collinear and overlapping
		glm::vec2 diff = ptSegA1 - ptSegB1;
		float cross = vDirA.x * diff.y - vDirA.y * diff.x;

		// If cross product is not zero, they are parallel but not collinear
		if(std::abs(cross) > HyMath::FloatSlop)
			return false;

		// Check if segments overlap along the line
		float t1 = (diff.x * vDirA.x + diff.y * vDirA.y) / (vDirA.x * vDirA.x + vDirA.y * vDirA.y);
		float t2 = t1 + (vDirB.x * vDirA.x + vDirB.y * vDirA.y) / (vDirA.x * vDirA.x + vDirA.y * vDirA.y);

		// Ensure the segments overlap in parameter space
		if(t1 > 1.0f || t2 < 0.0f)
			return false;

		// Set intersection point to the midpoint of overlapping region
		float t = std::max(0.0f, std::min(1.0f, t1));
		ptIntersectionOut = ptSegA1 + t * vDirA;
		return true;
	}

	// Calculate the parameters for intersection point
	glm::vec2 diff = ptSegA1 - ptSegB1;
	float t = (diff.x * vDirB.y - diff.y * vDirB.x) / fDenom;
	float u = (diff.x * vDirA.y - diff.y * vDirA.x) / fDenom;

	// Check if intersection point lies within both segments
	if(t >= 0.0f && t <= 1.0f && u >= 0.0f && u <= 1.0f)
	{
		ptIntersectionOut = ptSegA1 + t * vDirA;
		return true;
	}

	return false;
}

/*static*/ float HyMath::NormalizeRange(float fValue, float fMin, float fMax)
{
	float fWidth = fMax - fMin;
	if(fWidth == 0.0f)
		return fMin;

	float fOffsetValue = fValue - fMin;
	return (fOffsetValue - (floor(fOffsetValue / fWidth) * fWidth)) + fMin;	// + fMin to reset back to start of original range
}

/*static*/ int32 HyMath::NormalizeRange(int32 iValue, int32 iMin, int32 iMax)
{
	int32 iWidth = iMax - iMin;
	if(iWidth == 0)
		return iMin;

	int32 iOffsetValue = iValue - iMin;
	return (iOffsetValue - ((iOffsetValue / iWidth) * iWidth)) + iMin;	// + iMin to reset back to start of original range
}

/*static*/ float HyMath::TweenProgress(float fStart, float fEnd, float fElaspedTime, float fFullDuration, HyTweenFunc fpTweenFunc /*= HyTween::Linear*/)
{
	fElaspedTime = HyMath::Clamp(fElaspedTime, 0.0f, fFullDuration);
	return fStart + (fEnd - fStart) * fpTweenFunc(fElaspedTime / fFullDuration);
}
