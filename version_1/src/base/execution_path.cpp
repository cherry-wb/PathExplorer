#include "execution_path.h"

#include <functional>

/**
 * @brief calculate join (least upper bound or cartesian product) of two cfi conditions
 */
addrint_value_maps_t join_maps(const addrint_value_maps_t& cond_a,
                               const addrint_value_maps_t& cond_b)
{
  addrint_value_maps_t joined_cond;

  std::for_each(cond_a.begin(), cond_a.end(), [&](addrint_value_maps_t::value_type a_map)
  {
    std::for_each(cond_b.begin(), cond_b.end(), [&](addrint_value_maps_t::value_type b_map)
    {
      addrint_value_map_t joined_map = a_map;
      bool is_conflict = false;

      std::for_each(b_map.begin(), b_map.end(),
                    [&](addrint_value_maps_t::value_type::value_type b_point)
      {
        if (!is_conflict)
        {
          // verify if the source of b_point exists in the a_map
          if (a_map.find(b_point.first) != a_map.end())
          {
            // exists, then verify if there is a conflict between corresponding targets
            // in a_map and b_map
            if (a_map[b_point.first] != b_map[b_point.first])
            {
              is_conflict = true;
            }
          }
          else
          {
            // does not exist, then add b_point into the joined map
            joined_map.insert(b_point);
          }
        }
      });

      if (!is_conflict) joined_cond.push_back(joined_map);
    });
  });

  return joined_cond;
}


typedef std::function<condition_t ()> lazy_func_cond_t;

/**
 * @brief higher_join_if_needed
 */
lazy_func_cond_t higher_join_if_needed(lazy_func_cond_t in_cond)
{
  auto new_cond = [&]() -> condition_t
  {
    condition_t real_in_cond = in_cond();
    return real_in_cond;
  };
  return new_cond;
}


/**
 * @brief higher_fix
 */
lazy_func_cond_t higher_fix(std::function<decltype(higher_join_if_needed)> join_func)
{
  // explicit Y combinator: Y f = f (Y f)
//  return join_func(fix(join_func));

  // implicit Y combinator: Y f = f (\x -> (Y f) x)
  return std::bind(join_func, std::bind(&higher_fix, join_func))();
}


/**
 * @brief join_if_needed
 */
condition_t join_maps_in_condition(condition_t in_cond)
{
  return in_cond;
}


/**
 * @brief fix
 */
condition_t y_fix(std::function<decltype(join_maps_in_condition)> join_func)
{
  // explicit Y combinator: Y f = f (Y f)
//  return join_func(fix(join_func));

  // implicit Y combinator: Y f = f (\x -> (Y f) x)
  return std::bind(join_func, std::bind(&y_fix, join_func))();
}


/**
 * @brief fix
 */
condition_t fix(const condition_t& prev_cond,
                std::function<decltype(join_maps_in_condition)> join_func)
{
  auto have_intersection = [&](const addrint_value_map_t& map_a,
                               const addrint_value_map_t& map_b) -> bool
  {
    bool intersection_detected = false;
    std::for_each(map_a.begin(), map_a.end(), [&](addrint_value_map_t::value_type map_a_elem)
    {
      if (!intersection_detected)
        intersection_detected = (map_b.find(map_a_elem.first) != map_b.end());
    });
    return intersection_detected;
  };

  bool intersection_detected = false;
  auto cond_elem_a = prev_cond.begin(); auto cond_elem_b = std::next(cond_elem_a);
  for (; cond_elem_a != prev_cond.end(); ++cond_elem_a)
  {
    for (; cond_elem_b != prev_cond.end(); ++cond_elem_b)
    {
      if (have_intersection(*(cond_elem_a->begin()), *(cond_elem_b->begin())))
      {
        auto joined_map = join_maps(*cond_elem_a, *cond_elem_b);


        break;
      }

      intersection_detected = have_intersection(*(cond_elem_a->begin()), *(cond_elem_b->begin()));
      if (intersection_detected) break;
    }
  }

  condition_t new_cond = prev_cond;
  if (intersection_detected)
  {


  }
  else
  {
    return std::move(prev_cond);
  }



  return std::move(std::bind(&fix, new_cond, join_func)());
}


/**
 * @brief constructor
 */
execution_path::execution_path(const order_ins_map_t& current_path,
                               const path_code_t& current_path_code)
{
  this->content = current_path;
  this->code = current_path_code;
}


/**
 * @brief reconstruct path condition
 */
void execution_path::calculate_condition()
{

  typedef decltype(this->content) ins_at_order_t;
//  ins_at_order_t::mapped_type prev_ins;
  std::for_each(this->content.begin(), this->content.end(), [&](ins_at_order_t::value_type order_ins)
  {

  });

  return;
}
