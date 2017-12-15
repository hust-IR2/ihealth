#ifndef __SORT_H__
#define __SORT_H__

#include "Collection.h"

namespace Ext
{
	namespace Collection
	{
		template<typename T>
		class CSort
		{
		private:
			typedef  int(*CompareFunc)(const T &a, const T &b);
			CompareFunc m_pComparor;

		public:
			CSort(CompareFunc comparor);

		private:
			// 下面的方法用于堆排序 HeapSort():
			int  LeftChild(const int i);
			void PercDown(T x[], int i, const int n);
			void Swap(T *l, T *r);

			// 下面的方法用于合并排序 MergeSort():
			void MSort(T x[], T tmp[], int left, int right);
			void Merge(T x[], T tmp[], int lpos, int rpos, int rightend);

			// 快速排序的中分 QuickSort():
			T Median3(T x[], const int left, const int right);

		public:
			void InsertSort(T x[], const int n);
			void ShellSort(T x[], const int n);
			void HeapSort(T x[], const int n);
			void MergeSort(T x[], int n);
			void QuickSort(T x[], int left, int right);
		};

		template<typename T>
		inline CSort<T>::CSort(CompareFunc comparor)
		{
			m_pComparor = comparor;
		}

		template<typename T>
		inline void CSort<T>::InsertSort(T x[], const int n)
		{
			int i;
			int j;
			T tmp;

			for (i = 0; i < n; ++i)
			{
				tmp = x[i];             // copy it first
				for (j = i; j > 0; --j) // unsorted region; (0 ~ (i - 1)) is sorted
					//if (x[j - 1] > tmp)
					if (m_pComparor(x[j-1], tmp) > 0)
						x[j] = x[j - 1];// move back elements to empty a right position
					else
						break;          // we got it! x[j] is the right position
				x[j] = tmp;             // place it to the right position
			}
		}

		template<typename T>
		inline void CSort<T>::ShellSort(T x[], const int n)
		{
			int i;
			int j;
			int nIncrement;
			T tmp;

			for (nIncrement = n / 2; nIncrement > 0; nIncrement /= 2)
			{
				for (i = nIncrement; i < n; ++i)
				{
					tmp = x[i];
					for (j = i; j >= nIncrement; j -= nIncrement)
					{
						if (tmp < x[j - nIncrement])
							x[j] = x[j - nIncrement];
						else
							break;
					}
					x[j] = tmp;
				}
			}
		}

		template<typename T>
		inline int CSort<T>::LeftChild(const int i)
		{
			return (2 * i + 1);
		}

		template<typename T>
		inline void CSort<T>::PercDown(T x[], int i, const int n)
		{
			int nChild;
			T tmp;

			for (tmp = x[i]; LeftChild(i) < n; i = nChild)
			{
				nChild = LeftChild(i);
				//if ((nChild != n - 1) && (x[nChild + 1] > x[nChild]))
				if ((nChild != n - 1) && (m_pComparor(x[nChild+1], x[nChild]) > 0))
					++nChild;
				//if (tmp < x[nChild])
				if (m_pComparor(tmp, x[nChild]) < 0)
					x[i] = x[nChild];
				else
					break;
			}
			x[i] = tmp;
		}

		template<typename T>
		inline void CSort<T>::Swap(T *l, T *r)
		{
			T tmp = *l;
			*l = *r;
			*r = tmp;
		}

		template<typename T>
		inline void CSort<T>::HeapSort(T x[], const int n)
		{
			int i;

			for (i = n / 2; i >= 0; --i)    // build heap
				PercDown(x, i, n);
			for (i = n - 1; i > 0; --i)
			{
				Swap(&x[0], &x[i]);         // delete max
				PercDown(x, 0, i);
			}
		}

		template<typename T>
		inline void CSort<T>::Merge(T x[], T tmp[], int lpos, int rpos, int rightend)
		{
			int i;
			int leftend;
			int numelements;
			int tmppos;

			leftend = rpos - 1;
			tmppos = lpos;
			numelements = rightend - lpos + 1;

			// main loop
			while ((lpos <= leftend) && (rpos <= rightend))
			{
				if (x[lpos] <= x[rpos])
					tmp[tmppos++] = x[lpos++];
				else
					tmp[tmppos++] = x[rpos++];
			}

			while (lpos <= leftend)     // copy rest of first half
				tmp[tmppos++] = x[lpos++];
			while (rpos <= rightend)    // copy rest of second half
				tmp[tmppos++] = x[rpos++];

			// copy tmp back
			for (i = 0; i < numelements; ++i, --rightend)
				x[rightend] = tmp[rightend];
		}

		template<typename T>
		inline void CSort<T>::MSort(T x[], T tmp[], int left, int right)
		{
			int center;

			if (left < right)
			{
				center = (left + right) / 2;
				MSort(x, tmp, left, center);
				MSort(x, tmp, center + 1, right);
				Merge(x, tmp, left, center + 1, right);
			}
		}

		template<typename T>
		inline void CSort<T>::MergeSort(T x[], int n)
		{
			T *tmp;

			tmp = new (T[n * sizeof(T)]);
			if (NULL != tmp)
			{
				MSort(x, tmp, 0, n - 1);
				delete tmp;
			}
		}

		template<typename T>
		inline T CSort<T>::Median3(T x[], const int left, const int right)
		{
			int center = (left + right) / 2;

			//if (x[left] > x[center])
			if (m_pComparor(x[left], x[center]) > 0)
				Swap(&x[left], &x[center]);

			//if (x[left] > x[right])
			if (m_pComparor(x[left], x[right]) > 0)
				Swap(&x[left], &x[right]);

			//if (x[center] > x[right])
			if (m_pComparor(x[center], x[right]) > 0)
				Swap(&x[center], &x[right]);

			// invariant: x[left] <= x[center] <= x[right]

			Swap(&x[center], &x[right - 1]);    // hide pivot
			return x[right - 1];                // return pivot
		}

		template<typename T>
		inline void CSort<T>::QuickSort(T x[], int left, int right)
		{
			int i;
			int j;
			int cutoff = 3;
			T pivot;

			if (left + cutoff <= right)
			{
				pivot = Median3(x, left, right);
				i = left;
				j = right - 1;
				for (;;)
				{
					//while (x[++i] < pivot) {}
					//while (x[--j] > pivot) {}
					while (m_pComparor(x[++i], pivot) < 0) {}
					while (m_pComparor(x[--j], pivot) > 0) {}
					if (i < j)
						Swap(&x[i], &x[j]);
					else
						break;
				}
				Swap(&x[i], &x[right - 1]); // restore pivot
				QuickSort(x, left, i - 1);
				QuickSort(x, i + 1, right);
			}
			else    // do an insertion sort on the subarray
				InsertSort(x + left, right - left + 1);
		}

	}// End namespace Collection
}// End namespace Ext;

#endif  // __SORT_H__