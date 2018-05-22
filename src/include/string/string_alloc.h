//
//  xpalloc.h
//
//  Created by mgao on 2015-12-31.
//  Copyright (c) 2015年 mgao. All rights reserved.
//
//  This class be used as complement of stack alloc, if size to alloc
//  be too large from stack, we should consider to malloc buf from heap.
//
#pragma once
//#include "xpbase.h"
#include <string.h>

class xp_alloc_buf_mgr
{
public :
	xp_alloc_buf_mgr() : m_header(nullptr)
	{
	}
	void* alloca( int size)
	{
		alloc_buf_node* p = (alloc_buf_node*)::malloc(size + sizeof(alloc_buf_node));
		if ( p == nullptr )
			return nullptr;

		p->m_next = m_header;
		m_header = p;
		return p->get_data();
	}

	~xp_alloc_buf_mgr()
	{
		// walk the list and free the buffers
		while ( m_header != nullptr )
		{
			alloc_buf_node* p = m_header;
			m_header = m_header->m_next;
			::free(p);
		}
	}

private :
	struct alloc_buf_node
	{
		alloc_buf_node* m_next;
		void* get_data()
		{
			return (this + 1);
		}
	};

	alloc_buf_node* m_header;
};

#ifndef USES_HEAP_ALLOCA
#define USES_HEAP_ALLOCA xp_alloc_buf_mgr _xp_safe_alloc_mgr
#endif