#pragma once

namespace BB
{
	class IBBProjectile;
	class IBBProjectileSolver
	{
	protected:
		template <class T = IBBProjectile>
		class ProjectileRange
		{
			static_assert(std::is_base_of<IBBProjectile, T>::value, "T must be a derived class of IBBProjectile");
			T* m_ptr;
			size_t m_count;
		public:
			ProjectileRange(T* InPtr, const size_t InCount)
				: m_ptr(InPtr), m_count(InCount) {
			}
			T* begin() const { return m_ptr; }
			T* end() const { return m_ptr + m_count; }
		};

	protected:
		IBBProjectileSolver() = default;
		virtual ~IBBProjectileSolver() {};
	public:
		virtual bool SolveProjectiles(IBBProjectile* InProjectileArray, size_t nbElements) = 0;
	};
};