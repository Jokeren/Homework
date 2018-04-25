// Determinant calculation module

pub const MATRIX_LEN: usize = 32;


pub struct Fraction {
  pub numerator: i128,
  pub denominator: i128,
}


impl Copy for Fraction { }


impl Clone for Fraction {
  fn clone(&self) -> Fraction {
    *self
  }
}


fn i128abs(num: i128) -> i128 {
  if num < 0 {
    return -num;
  } else {
    return num;
  }
}


fn i128div(mut l: i128, mut r: i128) -> i128 {
  return l / r;
  //let mut signed : bool = false;
  //if (l < 0 && r > 0) || (l > 0 && r < 0) {
  //  signed = true;
  //}
  //l = i128abs(l);
  //r = i128abs(r);

  //if r > l {
  //  if signed {
  //    return -1;
  //  } else {
  //    return 0;
  //  }
  //}

  //if r == l {
  //  if signed {
  //    return -1;
  //  } else {
  //    return 1;
  //  }
  //}

  //let mut current : i128 = 1;
  //let mut dividend : i128 = l;
  //let mut divisor : i128 = r;
  //let mut result : i128 = 0;

  //while dividend >= divisor {
  //  current = current << 1;
  //  divisor = divisor << 1;   
  //}

  //current = current >> 1;
  //divisor = divisor >> 1;

  //while current != 0 {
  //  if dividend >= divisor {
  //    dividend = dividend - divisor;
  //    result = current | result;
  //  }
  //  current = current >> 1;
  //  divisor = divisor >> 1;
  //}

  //if dividend == 0 {
  //  if signed {
  //    return -result;
  //  } else {
  //    return result;
  //  }
  //} else {
  //  if signed {
  //    return -(result + 1);
  //  } else {
  //    return result;
  //  }
  //}
}


fn i128mod(l: i128, r: i128) -> i128 {
  return l - i128div(l, r) * r;
}


fn i128gcd(mut l: i128, mut r: i128) -> i128 {
  if l < r {
    let tmp: i128 = l;
    l = r;
    r = tmp;
  }
  while r != 0 {
    let tmp: i128 = r;
    r = i128mod(l, r);
    l = tmp;
  }
  return l; 
}


impl Fraction {
  fn to_i128(&self) -> i128 {
    return i128div(self.numerator, self.denominator);
  }

  fn norm(&self) -> Fraction {
    if self.numerator == 0 {
      return Fraction {numerator: 0, denominator: 1};
    }
    let m_gcd = i128gcd(i128abs(self.numerator), i128abs(self.denominator));
    let fraction = Fraction { numerator: i128div(self.numerator, m_gcd), denominator: i128div(self.denominator, m_gcd) };
    return fraction;
  }

  fn is_zero(&self) -> bool {
    if self.numerator == 0 {
      return true;
    } else {
      return false;
    }
  }

  fn sub(&self, other: &Fraction) -> Fraction {
    let m_gcd = i128gcd(i128abs(self.denominator), i128abs(other.denominator));
    let m_lcm = self.denominator * (i128div(other.denominator, m_gcd));
    let self_numerator = self.numerator * (i128div(m_lcm, self.denominator));
    let other_numerator = other.numerator * (i128div(m_lcm, other.denominator));
    let fraction = Fraction { numerator: self_numerator - other_numerator, denominator: m_lcm };
    return fraction.norm();
  }

  fn mul(&self, other: &Fraction) -> Fraction {
    let fraction = Fraction { numerator: self.numerator * other.numerator, denominator: self.denominator * other.denominator };
    return fraction.norm();
  }

  fn div(self, other: &Fraction) -> Fraction {
    let fraction = Fraction { numerator: other.denominator, denominator: other.numerator};
    let fraction = Fraction { numerator: self.numerator * fraction.numerator, denominator: self.denominator * fraction.denominator};
    return fraction.norm();
  }
}


pub fn determinant_fn(matrix: &mut [Fraction; MATRIX_LEN * MATRIX_LEN])->i64 {
  let mut det: Fraction = Fraction { numerator: 1, denominator: 1};

  unsafe {
    for i in 0..MATRIX_LEN {
      let mut max_loc = i;
      for j in (i+1)..MATRIX_LEN {
        let zero = matrix.get_unchecked(j * MATRIX_LEN + i).is_zero();
        if zero == false {
          max_loc = j;
          break;
        }
      }

      if max_loc != i {
        det = det.mul(&Fraction { numerator: -1, denominator: 1});
        for j in i..MATRIX_LEN {
          let tmp = matrix.get_unchecked_mut(max_loc * MATRIX_LEN + j).clone();
          *matrix.get_unchecked_mut(max_loc * MATRIX_LEN + j) = matrix.get_unchecked(i * MATRIX_LEN + j).clone();
          *matrix.get_unchecked_mut(i * MATRIX_LEN + j) = tmp;
        }
      }

      for j in (i+1)..MATRIX_LEN {
        let fraction = matrix.get_unchecked(j * MATRIX_LEN + i).div(matrix.get_unchecked(i * MATRIX_LEN + i));
        for k in i..MATRIX_LEN {
          let tmp = fraction.mul(matrix.get_unchecked(i * MATRIX_LEN + k));
          *matrix.get_unchecked_mut(j * MATRIX_LEN + k) = matrix.get_unchecked(j * MATRIX_LEN + k).sub(&tmp);
        }
      }
      det = det.mul(matrix.get_unchecked_mut(i * MATRIX_LEN + i));
    }
  }

  let det: i64 = det.to_i128() as i64;
  return det;
}
