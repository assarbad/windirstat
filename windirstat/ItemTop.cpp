// ItemTop.cpp - Implementation of CItemTop
//
// WinDirStat - Directory Statistics
// Copyright (C) 2003-2005 Bernhard Seifert
// Copyright (C) 2004-2024 WinDirStat Team (windirstat.net)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "stdafx.h"
#include "ItemTop.h"
#include "FileTopControl.h"
#include "WinDirStat.h"
#include "MainFrame.h"
#include "Localization.h"

CItemTop::CItemTop(CItem* item) : m_Item(item) {}

bool CItemTop::DrawSubitem(const int subitem, CDC* pdc, const CRect rc, const UINT state, int* width, int* focusLeft) const
{
    // Handle individual file items
    if (subitem != COL_ITEMTOP_NAME) return false;
    return CTreeListItem::DrawSubitem(columnMap.at(subitem), pdc, rc, state, width, focusLeft);
}

std::wstring CItemTop::GetText(const int subitem) const
{
    // Root node
    static std::wstring tops = Localization::Lookup(IDS_LARGEST_FILES);
    if (GetParent() == nullptr) return subitem == COL_ITEMTOP_NAME ? tops : std::wstring{};

    // Parent hash nodes
    if (m_Item == nullptr) return {};

    // Individual file names
    if (subitem == COL_ITEMTOP_NAME) return m_Item->GetPath();
    return m_Item->GetText(columnMap.at(subitem));
}

int CItemTop::CompareSibling(const CTreeListItem* tlib, const int subitem) const
{
    // Root node
    if (GetParent() == nullptr) return 0;
    
    // Parent hash nodes
    if (m_Item == nullptr) return 0;

    // Individual file names
    const auto* other = reinterpret_cast<const CItemTop*>(tlib);
    return m_Item->CompareSibling(other->m_Item, columnMap.at(subitem));
}

int CItemTop::GetTreeListChildCount()const
{
    return static_cast<int>(m_Children.size());
}

CTreeListItem* CItemTop::GetTreeListChild(const int i) const
{
    return m_Children[i];
}

short CItemTop::GetImageToCache() const
{
    return (m_Item != nullptr) ? m_Item->GetImageToCache() :
        GetIconImageList()->GetFreeSpaceImage();
}

int CItemTop::GetImage() const
{
    return (m_Item != nullptr) ? CTreeListItem::GetImage() :
        GetIconImageList()->GetFreeSpaceImage();
}

void CItemTop::AddTopItemChild(CItemTop* child)
{
    child->SetParent(this);

    std::lock_guard guard(m_Protect);
    m_Children.push_back(child);

    if (IsVisible() && IsExpanded())
    {
        CFileTopControl::Get()->OnChildAdded(this, child);
    }
}

void CItemTop::RemoveTopItemChild(CItemTop* child)
{
    std::lock_guard guard(m_Protect);
    std::erase(m_Children, child);

    if (IsVisible())
    {
        CFileTopControl::Get()->OnChildRemoved(this, child);
    }

    delete child;
}
