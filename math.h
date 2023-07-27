#pragma once

namespace math {
    // pi constants.
    constexpr float pi = 3.1415926535897932384f; // pi
    constexpr float pi_2 = pi * 2.f;               // pi * 2
#define M_PI		(float)3.14159265358979323846f
#define M_RADPI		57.295779513082f
#define M_PIRAD     0.01745329251f
#define RAD2DEG( x  )  ( (float)(x) * (float)(180.f / M_PI) )
#define DEG2RAD( x  )  ( (float)(x) * (float)(M_PI / 180.f) )

    // degrees to radians.
    __forceinline constexpr float deg_to_rad( float val ) {
        return val * ( pi / 180.f );
    }

    // radians to degrees.
    __forceinline constexpr float rad_to_deg( float val ) {
        return val * ( 180.f / pi );
    }

    // angle mod ( shitty normalize ).
    __forceinline float AngleMod( float angle ) {
        return ( 360.f / 65536 ) * ( ( int )( angle * ( 65536.f / 360.f ) ) & 65535 );
    }

    typedef __declspec(align(16)) union {
        float f[4];
        __m128 v;
    } m128;

    inline __m128 sqrt_ps(const __m128 squared) {
        return _mm_sqrt_ps(squared);
    }

    void AngleMatrix( const ang_t& ang, const vec3_t& pos, matrix3x4_t& out );

    // normalizes an angle.
    void NormalizeAngle( float &angle );

    __forceinline float NormalizedAngle( float angle ) {
        NormalizeAngle( angle );
        return angle;
    }

    float ApproachAngle( float target, float value, float speed );
    void  VectorAngles( const vec3_t& forward, ang_t& angles, vec3_t* up = nullptr );
    inline void SinCos(float radians, float* sine, float* cosine);
    void matrix_set_column(const vec3_t& in, int column, matrix3x4_t& out);
    vec3_t vector_rotate(const vec3_t& in1, const matrix3x4_t& in2);
    void angle_matrix(const ang_t& angles, const vec3_t& position, matrix3x4_t& matrix);
    void angle_matrix(const ang_t& angles, matrix3x4_t& matrix);
    vec3_t vector_angles(const vec3_t& v);
    vec3_t angle_vectors(const vec3_t& angles);
    void  AngleVectors( const ang_t& angles, vec3_t* forward, vec3_t* right = nullptr, vec3_t* up = nullptr );
    float GetFOV( const ang_t &view_angles, const vec3_t &start, const vec3_t &end );
    void  VectorTransform( const vec3_t& in, const matrix3x4_t& matrix, vec3_t& out );
    void  VectorITransform( const vec3_t& in, const matrix3x4_t& matrix, vec3_t& out );
    void  MatrixAngles( const matrix3x4_t& matrix, ang_t& angles );
    float AngleDiff(float destAngle, float srcAngle);
    void  MatrixCopy( const matrix3x4_t &in, matrix3x4_t &out );
    vec3_t extrapolate_pos(vec3_t pos, vec3_t extension, int amount, float interval);
    float NormalizeYaw(float angle);
    void  ConcatTransforms( const matrix3x4_t &in1, const matrix3x4_t &in2, matrix3x4_t &out );
    bool IntersectSegmentToSegment(vec3_t s1, vec3_t s2, vec3_t k1, vec3_t k2, float radius);
    bool IntersectionBoundingBox(const vec3_t& start, const vec3_t& dir, const vec3_t& min, const vec3_t& max, vec3_t* hit_point = nullptr);

    // computes the intersection of a ray with a box ( AABB ).
    float SegmentToSegment(const vec3_t s1, const vec3_t s2, const vec3_t k1, const vec3_t k2);
    bool IntersectRayWithBox( const vec3_t &start, const vec3_t &delta, const vec3_t &mins, const vec3_t &maxs, float tolerance, BoxTraceInfo_t *out_info );
    bool IntersectRayWithBox( const vec3_t &start, const vec3_t &delta, const vec3_t &mins, const vec3_t &maxs, float tolerance, CBaseTrace *out_tr, float *fraction_left_solid = nullptr );

    // computes the intersection of a ray with a oriented box ( OBB ).
    bool IntersectRayWithOBB( const vec3_t &start, const vec3_t &delta, const matrix3x4_t &obb_to_world, const vec3_t &mins, const vec3_t &maxs, float tolerance, CBaseTrace *out_tr );
    bool IntersectRayWithOBB( const vec3_t &start, const vec3_t &delta, const vec3_t &box_origin, const ang_t &box_rotation, const vec3_t &mins, const vec3_t &maxs, float tolerance, CBaseTrace *out_tr );

    // returns whether or not there was an intersection of a sphere against an infinitely extending ray. 
    // returns the two intersection points.
    bool IntersectInfiniteRayWithSphere( const vec3_t &start, const vec3_t &delta, const vec3_t &sphere_center, float radius, float *out_t1, float *out_t2 );

    // returns whether or not there was an intersection, also returns the two intersection points ( clamped 0.f to 1.f. ).
    // note: the point of closest approach can be found at the average t value.
    bool IntersectRayWithSphere( const vec3_t &start, const vec3_t &delta, const vec3_t &sphere_center, float radius, float *out_t1, float *out_t2 );

	vec3_t Interpolate( const vec3_t from, const vec3_t to, const float percent );

    vec3_t CalcAngle(const vec3_t& vecSource, const vec3_t& vecDestination);

    template < typename t >
    __forceinline void clamp( t& n, const t& lower, const t& upper ) {
        n = std::max( lower, std::min( n, upper ) );
    }

	// mixed types involved.
	template < typename T >
	T Clamp(const T& val, const T& minVal, const T& maxVal) {
		if ((T)val < minVal)
			return minVal;
		else if ((T)val > maxVal)
			return maxVal;
		else
			return val;
	}

    template< class T, class Y >
    FORCEINLINE T clamp2(T const& val, Y const& minVal, Y const& maxVal)
    {
        if (val < (T const&)minVal)
            return (T const&)minVal;
        else if (val > (T const&)maxVal)
            return (T const&)maxVal;
        else
            return val;
    }

    template<class T, class U>
    static T dont_break(const T& in, const U& low, const U& high)
    {

        if (in <= low)
            return low;

        if (in >= high)
            return high;

        return in;
    }
}