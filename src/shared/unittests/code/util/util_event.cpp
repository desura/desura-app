/*
$LicenseInfo:firstyear=2014&license=lgpl$
Copyright (C) 2014, Linden Research, Inc.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation;
version 2.1 of the License only.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, see <http://www.gnu.org/licenses/>
or write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

Linden Research, Inc., 945 Battery Street, San Francisco, CA  94111  USA
$/LicenseInfo$
*/

#include <gtest/gtest.h>
#include "Common.h"

namespace UnitTest
{
	template <typename T>
	class TestCallback
	{
	public:
		void callback()
		{
			hitCallback = true;
		}

		void callbackRef(T& t)
		{
			pT = &t;
			vT = t;

			t = vNewVal;
			hitCallback = true;
		}

		void callbackCopy(T t)
		{
			pT = &t;
			vT = t;
			t = vNewVal;
			hitCallback = true;
		}

		T* pT = nullptr;
		T vT = T();
		T vNewVal = T();

		bool hitCallback = false;
	};


	TEST(Event, VoidCallback)
	{
		EventV event;
		TestCallback<int> a;

		event += delegate(&a, &TestCallback<int>::callback);

		event();
		EXPECT_TRUE(a.hitCallback);

		a.hitCallback = false;
		event -= delegate(&a, &TestCallback<int>::callback);

		event();
		EXPECT_FALSE(a.hitCallback);
	}

	TEST(Event, RefCallback)
	{
		Event<int> event;
		TestCallback<int> a;
		a.vNewVal = -33;

		event += delegate(&a, &TestCallback<int>::callbackRef);

		int val = 24;
		event(val);

		EXPECT_TRUE(a.hitCallback);
		EXPECT_EQ(-33, val);
		EXPECT_EQ(&val, a.pT);
		EXPECT_EQ(24, a.vT);

		a.hitCallback = false;
		event -= delegate(&a, &TestCallback<int>::callbackRef);

		event(val);
		EXPECT_FALSE(a.hitCallback);
	}

	TEST(Event, CopyCallback)
	{
		EventC<int> event;
		TestCallback<int> a;
		a.vNewVal = -33;

		event += delegate(&a, &TestCallback<int>::callbackCopy);

		int val = 24;
		event(val);

		EXPECT_TRUE(a.hitCallback);
		EXPECT_NE(-33, val);
		EXPECT_NE(&val, a.pT);
		EXPECT_EQ(24, a.vT);

		a.hitCallback = false;
		event -= delegate(&a, &TestCallback<int>::callbackCopy);

		event(val);
		EXPECT_FALSE(a.hitCallback);
	}

	TEST(Event, VoidProxy)
	{
		EventV event1;
		EventV event2;

		TestCallback<int> a;

		event2 += delegate(&a, &TestCallback<int>::callback);
		event1 += delegate(&event2);

		event1();
		EXPECT_TRUE(a.hitCallback);

		a.hitCallback = false;
		event1 -= delegate(&event2);

		event1();
		EXPECT_FALSE(a.hitCallback);
	}

	TEST(Event, RefProxy)
	{
		Event<int> event1;
		Event<int> event2;

		TestCallback<int> a;
		a.vNewVal = -33;

		event2 += delegate(&a, &TestCallback<int>::callbackRef);
		event1 += delegate(&event2);

		int val = 24;
		event1(val);

		EXPECT_TRUE(a.hitCallback);
		EXPECT_EQ(-33, val);
		EXPECT_EQ(&val, a.pT);
		EXPECT_EQ(24, a.vT);

		a.hitCallback = false;
		event1 -= delegate(&event2);

		event1(val);
		EXPECT_FALSE(a.hitCallback);
	}

	TEST(Event, CopyProxy)
	{
		EventC<int> event1;
		EventC<int> event2;
		TestCallback<int> a;
		a.vNewVal = -33;

		event2 += delegate(&a, &TestCallback<int>::callbackCopy);
		event1 += delegate(&event2);

		int val = 24;
		event1(val);

		EXPECT_TRUE(a.hitCallback);
		EXPECT_NE(-33, val);
		EXPECT_NE(&val, a.pT);
		EXPECT_EQ(24, a.vT);

		a.hitCallback = false;
		event1 -= delegate(&event2);

		event1(val);
		EXPECT_FALSE(a.hitCallback);
	}
}